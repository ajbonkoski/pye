#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include "common/common.h"
#include "common/strsafe.h"

#ifdef __cplusplus
extern "C" {
#endif

// an interim interface for the buffer_t to comunicate changes
// back to the screen_t so display_t can be properly updated
enum edit_result {
    ER_NONE, ER_CURSOR, ER_ALL
};

// this object wraps the messy usage of 2D gap_buffers into a nice interface.
// this also decouples the data storage and buffer manipulation logic.

// this decoupling is important because the buffer manipulation logic will
// ultimately get fairly messy (incl. and interface to the scripting layer)
// Thus, decoupling helps us avoid confusing the interfaces

typedef struct data_buffer data_buffer_t;
struct data_buffer
{
    void *impl;
    uint impl_type;

    void (*get_cursor)(data_buffer_t *this, uint *x, uint *y);
    void (*set_cursor)(data_buffer_t *this, uint x, uint y);

    // "just works" with the special chars (\n, backspace, del)
    void (*insert)(data_buffer_t *this, int c);

    // "is edited?" methods
    bool (*get_is_edited)(data_buffer_t *this);
    void (*set_is_edited)(data_buffer_t *this, bool e);

    // use databuf if != NULL, otherwise allocate memory
    // (if allocate, user is responsib;le for cleanup)
    char *(*get_line_data)(data_buffer_t *this, uint i, char *databuf);

    // this function allocates - user is responsible for deallocation
    strsafe_t *(*get_region_data)(data_buffer_t *this, uint sx, uint sy, uint ex, uint ey);
    void (*remove_region_data)(data_buffer_t *this, uint sx, uint sy, uint ex, uint ey);
    uint (*line_len)(data_buffer_t *this, uint i);
    char (*get_char_at)(data_buffer_t *this, uint x, uint y);
    uint (*num_lines)(data_buffer_t *this);

    // methods for screen, so it can figure out what to redraw
    enum edit_result (*get_edit_result)(data_buffer_t *this);
    void (*reset_edit_result)(data_buffer_t *this);

    void (*destroy)(data_buffer_t *this);
};

data_buffer_t *data_buffer_create(void);

#ifdef __cplusplus
}
#endif

#endif
