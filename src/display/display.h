#ifndef DISPLAY_H
#define DISPLAY_H

#include "common/common.h"
#include "common/varray.h"
#include "keyboard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint bg_rgb;
    uint fg_rgb;
    bool bold;
    bool underline;

} styles_t;

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
