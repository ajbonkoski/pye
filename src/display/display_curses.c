#include "display_curses.h"

#include <curses.h>
#include "common/varray.h"
#include "kbrd_curses.h"

#define IMPL_TYPE 0xb3627c50

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define MIN_WIDTH   10
#define MIN_HEIGHT   3

typedef struct
{
    display_t *super;
    bool good;
    bool running;

    // curses stuff
    WINDOW *wind;
    uint width;
    uint height;

    // key event delegates data
    kbrd_curses_t *kbrd;
    varray_t *key_delegates;  // a dynamic array of key_event_delegate_t structs

} display_curses_t;
static display_curses_t *cast_this(display_t *d)
{
    ASSERT(d->impl_type == IMPL_TYPE, "expected a display_curses object");
    return (display_curses_t *)d->impl;
}

typedef struct
{
    key_event_func_t key_callback;
    void *key_usr;

} key_event_delegate_t;

// forward declarations
static void _clear(display_t *disp);
static void get_size(display_t *disp, uint *w, uint *h);
static void set_cursor(display_t *disp, uint x, uint y);
static void get_cursor(display_t *disp, uint *x, uint *y);
static void register_kbrd_callback(display_t *disp, key_event_func_t f, void *usr);
static void trigger_key_callbacks(display_curses_t *this, key_event_t *e);
static void flush(display_t *disp);
static void main_loop(display_t *disp);
static void main_quit(display_t *disp);
static void _write(display_t *disp, const char *s, size_t num);
static void destroy(display_t *disp);
static display_curses_t *display_curses_create_internal(display_t *disp);

static void _clear(display_t *disp)
{
    clear();
    refresh();
}

static void get_size(display_t *disp, uint *w, uint *h)
{
    display_curses_t *this = cast_this(disp);
    *w = this->width;
    *h = this->height;
}

static void set_cursor(display_t *disp, uint x, uint y)
{
    move(x, y);
    refresh();
}

static void get_cursor(display_t *disp, uint *x, uint *y)
{
    display_curses_t *this = cast_this(disp);
    getyx(this->wind, *y, *x);
}

static void register_kbrd_callback(display_t *disp, key_event_func_t f, void *usr)
{
    display_curses_t *this = cast_this(disp);
    key_event_delegate_t *k = calloc(1, sizeof(key_event_delegate_t));
    k->key_callback = f;
    k->key_usr = usr;
    varray_add(this->key_delegates, k);
}

static void trigger_key_callbacks(display_curses_t *this, key_event_t *e)
{
    key_event_delegate_t *d;
    varray_iter(d, this->key_delegates) {
        if(d->key_callback(d->key_usr, e))
            break;
    }
}

static void flush(display_t *disp)
{

}

static void main_loop(display_t *disp)
{
    display_curses_t *this = cast_this(disp);
    this->running = true;

    while(this->good && this->running) {
        int c = getch();
        key_event_t ke_mem;
        key_event_t *ke = kbrd_curses_keycode(this->kbrd, c, &ke_mem);
        if(ke != NULL)
            trigger_key_callbacks(this, ke);
    }

    if(!this->good) {
        ERROR("Exiting mainloop due to errors\n");
        return;
    }
}

static void main_quit(display_t *disp)
{
    display_curses_t *this = cast_this(disp);
    this->running = false;
}

static void _write(display_t *disp, const char *s, size_t num)
{
    //display_terminal_t *this = cast_this(disp);
    for(size_t i = 0; i < num; i++) {
        if(s != NULL)
            addch((int)s[i]);
        else
            addch((int)' ');
    }
}

static void destroy(display_t *disp)
{
    display_curses_t *this = cast_this(disp);
    endwin();
    varray_map(this->key_delegates, free);
    varray_destroy(this->key_delegates);
    free(this);
    free(disp);
}

static display_curses_t *display_curses_create_internal(display_t *disp)
{
    display_curses_t *this = calloc(1, sizeof(display_curses_t));
    this->super = disp;
    this->good = true;
    this->running = false;
    this->key_delegates = varray_create();

    return this;
}

/* setup curses */
static void internal_initialize(display_t *disp)
{
    display_curses_t *this = cast_this(disp);
    this->wind = initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(this->wind, this->height, this->width);
    clear();
    refresh();

    set_crash_func((crash_func_t)endwin, NULL);
}

display_t *display_curses_create(void)
{
    display_t *d = calloc(1, sizeof(display_t));
    d->clear = _clear;
    d->set_cursor = set_cursor;
    d->get_cursor = get_cursor;
    d->get_size = get_size;
    d->register_kbrd_callback = register_kbrd_callback;
    d->flush = flush;
    d->main_loop = main_loop;
    d->main_quit = main_quit;
    d->write = _write;
    d->destroy = destroy;

    d->impl_type = IMPL_TYPE;
    d->impl = display_curses_create_internal(d);
    internal_initialize(d);

    return d;
}

