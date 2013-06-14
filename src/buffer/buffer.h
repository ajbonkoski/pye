#ifndef BUFFER_H
#define BUFFER_H

#include "common/common.h"
#include "data_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    bool is_formated;
    char *data;

    /* 'data' may have more characters due to formatting characters
       this variable provides the number of visible characters */
    uint num_visible;

} buffer_line_t;

static inline void buffer_line_destroy(buffer_line_t *bl)
{
    if(bl->data != NULL) free(bl->data);
    free(bl);
}

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
    buffer_line_t *(*get_line_data_fmt)(buffer_t *this, uint i);
    void (*register_formatter)(buffer_t *this, char *(*func)(void *usr, char *data), void *usr);
    uint (*num_lines)(buffer_t *this);
    enum edit_result (*input_key)(buffer_t *this, u32 c);

    void (*destroy)(buffer_t *this);


    // this is for the benefit of the scripting engine
    // any C-code unrelated to the scripting should be inside the
    // buffer_t implementation... this exposure is nesisary, such that
    // the scripts can behave as if they were injected directly into the
    // buffer_t module.
    data_buffer_t *(*get_data_buffer)(buffer_t *this);
};

buffer_t *buffer_create(void);

#ifdef __cplusplus
}
#endif

#endif
