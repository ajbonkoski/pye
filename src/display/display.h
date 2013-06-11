#ifndef DISPLAY_H
#define DISPLAY_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u32 modifiers;
    u32 key_code;
    u32 released;

} key_event_t;

#define KBRD_ARROW_LEFT  ((1)<<8)
#define KBRD_ARROW_RIGHT ((2)<<8)
#define KBRD_ARROW_UP    ((3)<<8)
#define KBRD_ARROW_DOWN  ((4)<<8)

// true means "handled", false means "not handled"
typedef bool (*key_event_func_t)(void *usr, key_event_t *e);

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
    void (*write)(display_t *this, const char *s, size_t num);
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
