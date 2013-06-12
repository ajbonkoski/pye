#ifndef BUFFER_H
#define BUFFER_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// an interim interface for the buffer_t to comunicate changes
// back to the screen_t so display_t can be properly updated
enum edit_result {
    ER_NONE, ER_CURSOR, ER_ALL
};

typedef struct buffer buffer_t;
struct buffer
{
    void *impl;
    uint impl_type;

    void (*get_cursor)(buffer_t *this, uint *x, uint *y);
    char *(*get_line_data)(buffer_t *this, uint i);
    uint (*num_lines)(buffer_t *this);
    enum edit_result (*input_key)(buffer_t *this, u32 c);
    void (*destroy)(buffer_t *this);
};

buffer_t *buffer_create(void);

#ifdef __cplusplus
}
#endif

#endif
