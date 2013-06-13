#ifndef BUFFER_H
#define BUFFER_H

#include "common/common.h"
#include "data_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct buffer buffer_t;
struct buffer
{
    void *impl;
    uint impl_type;

    void (*set_filename)(buffer_t *this, const char *filename);
    const char *(*get_filename)(buffer_t *this);
    void (*get_cursor)(buffer_t *this, uint *x, uint *y);
    void (*set_cursor)(buffer_t *this, uint x, uint y);
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
