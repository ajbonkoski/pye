#include "display_curses.h"

#include <curses.h>
#include "common/varray.h"
#include "kbrd_curses.h"

#define IMPL_TYPE 0xb3627c50

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define MIN_WIDTH   10
#define MIN_HEIGHT   3

#define START_COLOR  9
#define START_PAIR  9

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

    // styles
    varray_t *styles;

    // this is quite the hack
    // we just iterate around the available colors and pairs!
    int color_index;
    int pair_index;

    int current_color_pair;
    int current_color_fg;
    short old_r;
    short old_g;
    short old_b;

} display_curses_t;

// inline functions
static inline display_curses_t *cast_this(display_t *d)
{
    ASSERT(d->impl_type == IMPL_TYPE, "expected a display_curses object");
    return (display_curses_t *)d->impl;
}

static inline int get_new_color_index(display_curses_t *this)
{
    int ci = this->color_index;

    if(++this->color_index >= COLORS)
        this->color_index = START_COLOR;

    return ci;
}

static inline int get_new_pair_index(display_curses_t *this)
{
    int cp = this->pair_index;

    if(++this->pair_index >= COLOR_PAIRS)
        this->pair_index = START_PAIR;

    return cp;
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
static void set_styles(display_t *this, varray_t *styles);
static void remove_styles(display_t *this);
static void _write(display_t *disp, const char *s, size_t num, int style);
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
    move(y, x);
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

static void set_styles(display_t *disp, varray_t *styles)
{
    display_curses_t *this = cast_this(disp);
    this->styles = styles;
}

static void remove_styles(display_t *disp)
{
    display_curses_t *this = cast_this(disp);
    this->styles = NULL;  // don't free the mem (we aren't the owner)
}

static inline int irgb_to_red(uint rgb)
{
    uint red = (rgb>>16)&0xff;
    return (int)((double)red / 256.0 * 1000.0);
}

static inline int irgb_to_green(uint rgb)
{
    uint green = (rgb>>8)&0xff;
    return (int)((double)green / 256.0 * 1000.0);
}

static inline int irgb_to_blue(uint rgb)
{
    uint blue = (rgb>>0)&0xff;
    return (int)((double)blue / 256.0 * 1000.0);
}

static inline int decode_color_to_curses(uint color)
{
    switch(color) {
        case DISPLAY_COLOR_BLACK:   return COLOR_BLACK;
        case DISPLAY_COLOR_RED:     return COLOR_RED;
        case DISPLAY_COLOR_GREEN:   return COLOR_GREEN;
        case DISPLAY_COLOR_YELLOW:  return COLOR_YELLOW;
        case DISPLAY_COLOR_BLUE:    return COLOR_BLUE;
        case DISPLAY_COLOR_MAGENTA: return COLOR_MAGENTA;
        case DISPLAY_COLOR_CYAN:    return COLOR_CYAN;
        case DISPLAY_COLOR_WHITE:   return COLOR_WHITE;
        default:
            ASSERT_FAIL("unrecognized color code in decode_color_to_curses()");
            return -1;
    }
}

static inline void set_style(display_curses_t *this, display_style_t *style)
{
    int bg_curses_color = decode_color_to_curses(style->bg_color);
    int fg_curses_color = decode_color_to_curses(style->fg_color);

    int pi = get_new_pair_index(this);
    init_pair(pi, fg_curses_color, bg_curses_color);
    this->current_color_pair = pi;

    attron(COLOR_PAIR(this->current_color_pair));

    /* int ci = get_new_color_index(this); */
    /* int pi = get_new_pair_index(this); */
    /* color_content(ci, &this->old_r, &this->old_g, &this->old_b); */

    /* if(init_color(ci, 0, 0, 1000) == ERR) { */
    /*     ERROR("call to curses init_color() failed\n"); */
    /* } */
    /* this->current_color_fg = ci; */

    /* init_pair(pi, ci, COLOR_BLACK); */
    /* this->current_color_pair = pi; */

    /* attron(COLOR_PAIR(this->current_color_pair)); */
}

static inline void clear_style(display_curses_t *this)
{
    attroff(COLOR_PAIR(this->current_color_pair));
    /* init_color(this->current_color_fg, this->old_r, this->old_g, this->old_b); */
    this->current_color_pair = -1;
}

static void _write(display_t *disp, const char *s, size_t num, int style)
{
    display_curses_t *this = cast_this(disp);

    if(style != DISPLAY_STYLE_NONE) {
        ASSERT(this->styles != NULL, "tried to use a style before calling display->set_styles()");
        ASSERT(0 <= style && style < varray_size(this->styles), "varray out-of bounds in display->write()");
        display_style_t *st = varray_get(this->styles, style);
        set_style(this, st);
    }

    for(size_t i = 0; i < num; i++) {
        int c;
        if(s != NULL)
            c = (int)s[i];
        else
            c = (int)' ';
        addch(c);
    }

    doupdate();

    if(style != DISPLAY_STYLE_NONE)
        clear_style(this);
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
    this->color_index = START_COLOR;
    this->pair_index = START_PAIR;

    return this;
}

void list_colors(void)
{
    {
        short r, g, b;
        for(short i = 0; i < COLORS; i++) {
            color_content(i, &r, &g, &b);
            DEBUG("color %d: (%d, %d, %d)\n", i, r, g, b);
        }
    }

    {
        short f, b;
        for(short i = 0; i < 100; i++) {
            pair_content(i, &f, &b);
            DEBUG("pair %d: (%d, %d)\n", i, f, b);
        }
    }
}

/* setup curses */
static void internal_initialize(display_t *disp)
{
    display_curses_t *this = cast_this(disp);

    // curses doesn't seem to like changing colors for xterm... thus this very bad hack
    setenv("TERM", "xterm-256color", 1);

    this->wind = initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    getmaxyx(this->wind, this->height, this->width);
    clear();
    refresh();

    const char *term = getenv("TERM");
    DEBUG("Initialed ncurses with TERM='%s' and %d colors and %d pairs", term, COLORS, COLOR_PAIRS);

    list_colors();

    if(!can_change_color()) {
        ERROR("WRN: curses cannot change colors!\n");
    }

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
    d->set_styles = set_styles;
    d->remove_styles = remove_styles;
    d->write = _write;
    d->destroy = destroy;

    d->impl_type = IMPL_TYPE;
    d->impl = display_curses_create_internal(d);
    internal_initialize(d);

    return d;
}

