#ifndef DISPLAY_H
#define DISPLAY_H

#include "common/common.h"
#include "common/varray.h"
#include "keyboard.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_COLOR_BLACK    0
#define DISPLAY_COLOR_RED      1
#define DISPLAY_COLOR_GREEN    2
#define DISPLAY_COLOR_YELLOW   3
#define DISPLAY_COLOR_BLUE     4
#define DISPLAY_COLOR_MAGENTA  5
#define DISPLAY_COLOR_CYAN     6
#define DISPLAY_COLOR_WHITE    7

#define CHECK_COLOR(s, c) do { if(strcmp(str, s) == 0) return DISPLAY_COLOR_ ## c; } while(0)

static inline uint display_color_from_string(const char *str)
{
    CHECK_COLOR("black",   BLACK);
    CHECK_COLOR("red",     RED);
    CHECK_COLOR("green",   GREEN);
    CHECK_COLOR("yellow",  YELLOW);
    CHECK_COLOR("blue",    BLUE);
    CHECK_COLOR("magenta", MAGENTA);
    CHECK_COLOR("cyan",    CYAN);
    CHECK_COLOR("white",   WHITE);

    ERROR("unrecognized color '%s' in display_color_from_string()", str);
    return DISPLAY_COLOR_BLACK;
}

#undef CHECK_COLOR

#define DISPLAY_STYLE_NONE -1

typedef struct
{
    uint bg_color;
    bool bg_bright;
    uint fg_color;
    bool fg_bright;
    bool bold;
    bool underline;
    bool highlight;

} display_style_t;

static inline display_style_t *display_style_create_default(void)
{
    display_style_t *ds = calloc(1, sizeof(display_style_t));
    ds->bg_color = DISPLAY_COLOR_BLACK;
    ds->bg_bright = false;
    ds->fg_color = DISPLAY_COLOR_WHITE;
    ds->fg_bright = true;
    ds->bold = false;
    ds->underline = false;
    ds->highlight = false;

    return ds;
}

static inline void display_style_destroy(display_style_t *ds)
{
    // just a free() for now...
    free(ds);
}

typedef struct display display_t;
struct display
{
    void *impl;
    uint impl_type;

    void (*clear)(display_t *this);
    void (*set_cursor)(display_t *this, uint x, uint y);
    void (*get_cursor)(display_t *this, uint *x, uint *y);
    void (*get_size)(display_t *this, uint *w, uint *h);
    void (*register_kbrd_callback)(display_t *this, key_event_func_t f, void *usr);
    void (*set_styles)(display_t *this, varray_t *styles);
    void (*remove_styles)(display_t *this);
    void (*write)(display_t *this, const char *s, size_t num, int style); // style == -1 means "no style"
    void (*write_xy)(display_t *disp, uint x, uint y, const char *s, size_t num, int style);
    void (*flush)(display_t *this);
    void (*main_loop)(display_t *this);
    void (*main_quit)(display_t *this);
    void (*destroy)(display_t *this);
};

display_t *display_create_by_name(const char *name);

#ifdef __cplusplus
}
#endif

#endif
