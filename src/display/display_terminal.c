#include "display_terminal.h"

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

#define MIN_WIDTH   10
#define MIN_HEIGHT   3

typedef struct
{
    display_t *super;
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

} display_terminal_t;
display_terminal_t *cast_this(display_t *d)
{
    ASSERT(d->impl_type == IMPL_TYPE, "expected a display_terminal object");
    return (display_terminal_t *)d->impl;
}

typedef struct
{
    key_event_func_t key_callback;
    void *key_usr;

} key_event_delegate_t;

// forward declarations
static bool tcapopen(display_terminal_t *this);
static void clear(display_t *disp);
static void get_size(display_t *disp, uint *w, uint *h);
static void set_cursor(display_t *disp, uint x, uint y);
static void get_cursor(display_t *disp, uint *x, uint *y);
static void register_kbrd_callback(display_t *disp, key_event_func_t f, void *usr);
static void trigger_key_callbacks(display_terminal_t *this, key_event_t *e);
static void main_loop(display_t *disp);
static void main_quit(display_t *disp);
static void _write(display_t *disp, const char *s, size_t num);
static void destroy(display_t *disp);
static display_terminal_t *display_terminal_create_internal(display_t *s);


static bool tcapopen(display_terminal_t *this)
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
    set_crash_func((crash_func_t)ttclose, NULL);
    return this->open;
}

static void clear(display_t *disp)
{
    uint w, h;
    get_size(disp, &w, &h);
    set_cursor(disp, 0, 0);
    _write(disp, NULL, w*h);
    set_cursor(disp, 0, 0);
}

static void get_size(display_t *disp, uint *w, uint *h)
{
    struct winsize ws;
    ioctl (0, TIOCGWINSZ, &ws);
    *w = ws.ws_col;
    *h = ws.ws_row;

    ASSERT(*w >= MIN_WIDTH && *h >= MIN_HEIGHT, "The display is far to small...");
}

static void set_cursor(display_t *disp, uint x, uint y)
{
    display_terminal_t *this = cast_this(disp);
    ASSERT(this != NULL, "this should never happen");
    tputs(tgoto(this->CM, x, y), 1, ttputc);
    this->x = x;
    this->y = y;
}

static void get_cursor(display_t *disp, uint *x, uint *y)
{
    display_terminal_t *this = cast_this(disp);
    *x = this->x;
    *y = this->y;
}

static void register_kbrd_callback(display_t *disp, key_event_func_t f, void *usr)
{
    display_terminal_t *this = cast_this(disp);
    key_event_delegate_t *k = calloc(1, sizeof(key_event_delegate_t));
    k->key_callback = f;
    k->key_usr = usr;
    varray_add(this->key_delegates, k);
}


static void trigger_key_callbacks(display_terminal_t *this, key_event_t *e)
{
    key_event_delegate_t *d;
    varray_iter(d, this->key_delegates) {
        if(d->key_callback(d->key_usr, e))
            break;
    }
}

static void main_loop(display_t *disp)
{
    display_terminal_t *this = cast_this(disp);
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

static void main_quit(display_t *disp)
{
    display_terminal_t *this = cast_this(disp);
    this->running = false;
}

static void inc_cursor(display_terminal_t *this, size_t count)
{
    this->x += count;
    while(this->x >= this->width) {
        this->x -= this->width;
        this->y++;
    }
}

static void _write(display_t *disp, const char *s, size_t num)
{
    //display_terminal_t *this = cast_this(disp);
    for(size_t i = 0; i < num; i++) {
        if(s != NULL)
            ttputc((int)s[i]);
        else
            ttputc((int)' ');
        inc_cursor(cast_this(disp), 1);
    }

    ttflush();
}

static void destroy(display_t *disp)
{
    display_terminal_t *this = cast_this(disp);

    if(!ttclose())
        ERROR("Failed to properly close terminal\n");

    varray_map(this->key_delegates, free);
    varray_destroy(this->key_delegates);
    free(this);
    free(disp);
}

static display_terminal_t *display_terminal_create_internal(display_t *d)
{
    display_terminal_t *this = calloc(1, sizeof(display_terminal_t));
    this->super = d;
    this->good = true;
    this->running = false;
    this->key_delegates = varray_create();

    // open the terminal display
    if(!tcapopen(this)) {
        ERROR("Failed to open terminal display\n");
        this->good = false;
    }

    return this;
}

// stuff that can't/shouldn't be done during create()
void internal_initialize(display_t *disp)
{
    display_terminal_t *this = cast_this(disp);

    // do display init (like cursor placement and clearing)
    if(this->open == true) {
        get_size(disp, &this->width, &this->height);
        clear(disp);
    }
}

display_t *display_terminal_create(void)
{
    display_t *d = calloc(1, sizeof(display_t));
    d->clear = clear;
    d->set_cursor = set_cursor;
    d->get_cursor = get_cursor;
    d->get_size = get_size;
    d->register_kbrd_callback = register_kbrd_callback;
    d->main_loop = main_loop;
    d->main_quit = main_quit;
    d->write = _write;
    d->destroy = destroy;

    d->impl_type = IMPL_TYPE;
    d->impl = display_terminal_create_internal(d);

    internal_initialize(d);

    return d;
}

