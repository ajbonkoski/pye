#ifndef SCREEN_H
#define SCREEN_H

#include "common/common.h"
#include "common/varray.h"
#include "display/display.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct screen screen_t;
struct screen
{
    void *impl;
    uint impl_type;

    void (*add_buffer)(screen_t *this, void *buffer);
    varray_t *(*list_buffers)(screen_t *this);
    void (*set_buffer)(screen_t *this, uint id);
    void (*write_mb)(screen_t *this, const char *str); // write to the message bar
    void (*destroy)(screen_t *this);
};

screen_t *screen_create(display_t *disp);

#ifdef __cplusplus
}
#endif

#endif
