#ifndef SCREEN_H
#define SCREEN_H

#include "common/common.h"
#include "common/varray.h"
#include "display/display.h"
#include "buffer/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct screen screen_t;
struct screen
{
    void *impl;
    uint impl_type;

    // this will override the default handler
    void (*register_kbrd_callback)(screen_t *this, key_event_func_t f, void *usr);
    // returns the buffer's id
    uint (*add_buffer)(screen_t *this, buffer_t *buffer);
    // the varray returned should be considered constant (user should neither modify nor delete it)
    varray_t *(*list_buffers)(screen_t *this);
    void (*set_active_buffer)(screen_t *this, uint id);
    buffer_t *(*get_active_buffer)(screen_t *this);
    void (*write_mb)(screen_t *this, const char *str); // write to the message bar
    void (*destroy)(screen_t *this);
};

screen_t *screen_create(display_t *disp);

#ifdef __cplusplus
}
#endif

#endif
