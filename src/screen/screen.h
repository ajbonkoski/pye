#ifndef SCREEN_H
#define SCREEN_H

#include "common/common.h"
#include "common/varray.h"
#include "display/display.h"
#include "buffer/buffer.h"

#include "kill_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

// 'response' is internal memory, and should be copied if needed long-term
// Special cases: a NULL response means "canceled by user"
//                an empty string is just an empty response
typedef void (*mb_response_func_t)(void *usr, char *response);
typedef void (*buf_event_func_t)(void *usr, uint index);

typedef struct screen screen_t;
struct screen
{
    void *impl;
    uint impl_type;

    // essentially just alias's directly to display->
    void (*set_cursor)(screen_t *this, uint x, uint y);
    void (*get_cursor)(screen_t *this, uint *x, uint *y);

    // this will override the default handler
    void (*register_kbrd_callback)(screen_t *this, key_event_func_t f, void *usr);
    void (*register_buf_added_callback)(screen_t *this, buf_event_func_t f, void *usr);
    // returns the buffer's id
    uint (*add_buffer)(screen_t *this, buffer_t *buffer);
    // the varray returned should be considered constant (user should neither modify nor delete it)
    varray_t *(*list_buffers)(screen_t *this);
    buffer_t *(*get_buffer)(screen_t *this, uint id);
    void (*set_active_buffer)(screen_t *this, uint id);
    buffer_t *(*get_active_buffer)(screen_t *this);
    uint (*get_viewport_line)(screen_t *this);
    void (*mb_write)(screen_t *this, const char *str); // write to the message bar
    uint (*mb_get_yloc)(screen_t *this);
    void (*trigger_mode)(screen_t *this, const char *mode, ...);
    void (*destroy)(screen_t *this);

    kill_buffer_t *(*get_kill_buffer)(screen_t *this);

    // refresh/redraw the screen (mostly useful for the scripting environment)
    void (*refresh)(screen_t *this);
};

screen_t *screen_create(display_t *disp);

#ifdef __cplusplus
}
#endif

#endif
