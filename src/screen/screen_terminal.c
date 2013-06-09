#include "screen_terminal.h"

#include <sys/ioctl.h>
#include <term.h>

#include "common/timeutil.h"
#include "common/varray.h"
#include "termio.h"

#define IMPL_TYPE 0xb62ccfb6

/* Many functions in this file were adapted from a MicroEmacs variant.
   (pEmacs: https://github.com/hughbarney/pEmacs on 6/7/2013) */

#define TCAPSLEN 64
#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct
{
    screen_t *super;
    bool good;
    bool running;

    char *CM, *CE, *CL, *SO, *SE;
    char tcapbuf[TCAPSLEN];     /* capabilities actually used */
    bool revexist;
    bool eolexist;               /* does clear to EOL exist */
    bool open;

    uint width;
    uint height;
    uint x;
    uint y;

    // key event delegates data
    varray_t *key_delegates;  // a dynamic array of key_event_delegate_t structs

} screen_terminal_t;
screen_terminal_t *cast_this(screen_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a screen_terminal object");
    return (screen_terminal_t *)s->impl;
}

typedef struct
{
    key_event_func_t key_callback;
    void *key_usr;

} key_event_delegate_t;

// forward declarations
static bool tcapopen(screen_terminal_t *this);
static void clear(screen_t *scrn);
static void get_size(screen_t *scrn, uint *w, uint *h);
static void set_cursor(screen_t *scrn, uint x, uint y);
static void get_cursor(screen_t *scrn, uint *x, uint *y);
static void register_kbrd_callback(screen_t *scrn, key_event_func_t f, void *usr);
static void trigger_key_callbacks(screen_terminal_t *this, key_event_t *e);
static void main_loop(screen_t *scrn);
static void main_quit(screen_t *scrn);
static void _write(screen_t *scrn, const char *s, size_t num);
static void destroy(screen_t *scrn);
static screen_terminal_t *screen_terminal_create_internal(screen_t *s);


static bool tcapopen(screen_terminal_t *this)
{
    ASSERT(this->open == false, "Terminal is already open");

    // initialization
    this->revexist = false;
    this->eolexist = true;

    char *tv_stype;
    if((tv_stype = getenv("TERM")) == NULL) {
        ERROR("Environment variable TERM not defined\n");
        return false;
    }

    char tcbuf[1024];
    if((tgetent (tcbuf, tv_stype)) != 1) {
        ERROR("Unknown terminal type %s\n", tv_stype);
        return false;
    }

    char *p = this->tcapbuf;
    this->CL = tgetstr ("cl", &p);
    this->CM = tgetstr ("cm", &p);
    this->CE = tgetstr ("ce", &p);
    this->SE = tgetstr ("se", &p);
    this->SO = tgetstr ("so", &p);

    if(this->CE == NULL)
        this->eolexist = false;
    if(this->SO != NULL && this->SE != NULL)
        this->revexist = true;
    if(this->CL == NULL || this->CM == NULL) {
        ERROR("Insufficient termcap! (needs cl & cm abilities)\n");
        return false;
    }
    if (p >= &this->tcapbuf[TCAPSLEN])	/* XXX */
    {
        ERROR("Terminal description too big!\n");
        return false;
    }

    this->open = ttopen();
    DEBUG("Terminal Opening: %s", this->open ? "SUCCESS" : "FAILURE");

    // set the crash_func()
    set_crash_func((crash_func_t)destroy, this->super);
    return this->open;
}

static void clear(screen_t *scrn)
{
    uint w, h;
    get_size(scrn, &w, &h);
    set_cursor(scrn, 0, 0);
    _write(scrn, NULL, w*h);
    set_cursor(scrn, 0, 0);
}

static void get_size(screen_t *scrn, uint *w, uint *h)
{
    struct winsize ws;
    ioctl (0, TIOCGWINSZ, &ws);
    *w = ws.ws_col;
    *h = ws.ws_row;
}

static void set_cursor(screen_t *scrn, uint x, uint y)
{
    screen_terminal_t *this = cast_this(scrn);
    ASSERT(this != NULL, "this should never happen");
    tputs(tgoto(this->CM, x, y), 1, ttputc);
    this->x = x;
    this->y = y;
}

static void get_cursor(screen_t *scrn, uint *x, uint *y)
{
    screen_terminal_t *this = cast_this(scrn);
    *x = this->x;
    *y = this->y;
}

static void register_kbrd_callback(screen_t *scrn, key_event_func_t f, void *usr)
{
    screen_terminal_t *this = cast_this(scrn);
    key_event_delegate_t *k = calloc(1, sizeof(key_event_delegate_t));
    k->key_callback = f;
    k->key_usr = usr;
    varray_add(this->key_delegates, k);
}


static void trigger_key_callbacks(screen_terminal_t *this, key_event_t *e)
{
    key_event_delegate_t *d;
    varray_iter(d, this->key_delegates) {
        if(d->key_callback(d->key_usr, e))
            break;
    }
}

static void main_loop(screen_t *scrn)
{
    screen_terminal_t *this = cast_this(scrn);
    this->running = true;

    while(this->good && this->running) {
        int c = ttgetc();
        key_event_t ke;
        memset(&ke, 0, sizeof(key_event_t));
        ke.key_code = (u32)c;
        trigger_key_callbacks(this, &ke);
    }

    if(!this->good) {
        ERROR("Exiting mainloop due to errors\n");
        return;
    }
}

static void main_quit(screen_t *scrn)
{
    screen_terminal_t *this = cast_this(scrn);
    this->running = false;
}

static void inc_cursor(screen_terminal_t *this, size_t count)
{
    this->x += count;
    if(this->x > this->width) {
        this->x -= this->width;
        this->y++;
    }
}

static void _write(screen_t *scrn, const char *s, size_t num)
{
    //screen_terminal_t *this = cast_this(scrn);
    for(size_t i = 0; i < num; i++) {
        if(s != NULL)
            ttputc((int)s[i]);
        else
            ttputc((int)' ');
        inc_cursor(cast_this(scrn), 1);
    }

    ttflush();
}

static void destroy(screen_t *scrn)
{
    screen_terminal_t *this = cast_this(scrn);

    if(!ttclose())
        ERROR("Failed to properly close terminal\n");

    varray_map(this->key_delegates, free);
    varray_destroy(this->key_delegates);
    free(this);
    free(scrn);
}

static screen_terminal_t *screen_terminal_create_internal(screen_t *s)
{
    screen_terminal_t *this = calloc(1, sizeof(screen_terminal_t));
    this->super = s;
    this->good = true;
    this->running = false;
    this->key_delegates = varray_create();

    // open the terminal screen
    if(!tcapopen(this)) {
        ERROR("Failed to open terminal screen\n");
        this->good = false;
    }

    return this;
}

// stuff that can't/shouldn't be done during create()
void internal_initialize(screen_t *scrn)
{
    screen_terminal_t *this = cast_this(scrn);

    // do screen init (like cursor placement and clearing)
    if(this->open == true) {
        get_size(scrn, &this->width, &this->height);
        clear(scrn);
    }
}

screen_t *screen_terminal_create(void)
{
    screen_t *s = calloc(1, sizeof(screen_t));
    s->clear = clear;
    s->set_cursor = set_cursor;
    s->get_cursor = get_cursor;
    s->get_size = get_size;
    s->register_kbrd_callback = register_kbrd_callback;
    s->main_loop = main_loop;
    s->main_quit = main_quit;
    s->write = _write;
    s->destroy = destroy;

    s->impl_type = IMPL_TYPE;
    s->impl = screen_terminal_create_internal(s);

    internal_initialize(s);

    return s;
}

