#ifndef SCREEN_H
#define SCREEN_H

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

// true means "handled", false means "not handled"
typedef bool (*key_event_func_t)(void *usr, key_event_t *e);

typedef struct screen screen_t;
struct screen
{
    void *impl;
    uint impl_type;

    void (*clear)(screen_t *this);
    void (*set_cursor)(screen_t *this, uint x, uint y);
    void (*get_cursor)(screen_t *this, uint *x, uint *y);
    void (*register_kbrd_callback)(screen_t *this, key_event_func_t f, void *usr);
    void (*write)(screen_t *this, const char *s, size_t num);
    void (*main_loop)(screen_t *this);
    void (*main_quit)(screen_t *this);
    void (*destroy)(screen_t *this);
};

screen_t *screen_create_by_name(const char *name);

#ifdef __cplusplus
}
#endif

#endif
