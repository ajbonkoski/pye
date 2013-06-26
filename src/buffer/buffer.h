#ifndef BUFFER_H
#define BUFFER_H

#include "common/common.h"
#include "display/display.h"
#include "data_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

// highlight styles
#define HIGHLIGHT_STYLE_NONE      0
#define HIGHLIGHT_STYLE_NORMAL    1
#define HIGHLIGHT_STYLE_STANDOUT  2

typedef struct
{
    uint start_index;
    uint length;

    // Serves a dual purpose:
    //    used to specify the highlighted area before the formatter
    //    used to index 'styles' array after the formatter
    uint style_id;

} buffer_line_region_t;

static inline void buffer_line_region_destroy(buffer_line_region_t *blr)
{
    // just a free() for now...
    free(blr);
}

static inline buffer_line_region_t *buffer_line_region_create_default(void)
{
    buffer_line_region_t *blr = calloc(1, sizeof(buffer_line_region_t));
    blr->start_index = UINT_MAX;
    blr->length = UINT_MAX;
    blr->style_id = UINT_MAX;

    return blr;
}

static inline bool buffer_line_region_valid(buffer_line_region_t *blr)
{
    return blr->start_index != UINT_MAX &&
           blr->length      != UINT_MAX &&
           blr->style_id    != UINT_MAX;
}

typedef struct
{
    varray_t *styles;  // a list of "display_style_t" structs
    varray_t *regions; // a list of "buffer_line_region_t" structs
    char *data;

} buffer_line_t;

static inline void buffer_line_destroy(buffer_line_t *bl)
{
    if(bl->styles != NULL) {
        varray_map(bl->styles, (void (*)(void *))display_style_destroy);
        varray_destroy(bl->styles);
    }

    if(bl->regions != NULL) {
        varray_map(bl->regions, (void (*)(void *))buffer_line_region_destroy);
        varray_destroy(bl->regions);
    }

    if(bl->data != NULL)
        free(bl->data);

    free(bl);
}


typedef buffer_line_t *(*format_func_t)(void *usr, buffer_line_t *data);

typedef struct buffer buffer_t;
struct buffer
{
    void *impl;
    uint impl_type;

    bool (*get_mark)(buffer_t *this, uint *x, uint *y);
    void (*set_mark)(buffer_t *this, uint x, uint y);
    void (*clear_mark)(buffer_t *this);

    void (*set_filename)(buffer_t *this, const char *filename);
    const char *(*get_filename)(buffer_t *this);

    void (*get_cursor)(buffer_t *this, uint *x, uint *y);
    void (*set_cursor)(buffer_t *this, uint x, uint y);

    void (*goto_line_start)(buffer_t *this);
    void (*goto_line_end)(buffer_t *this);
    char *(*get_line_data)(buffer_t *this, uint i);
    buffer_line_t *(*get_line_data_fmt)(buffer_t *this, uint i);
    void (*register_formatter)(buffer_t *this, format_func_t func, void *usr);
    uint (*num_lines)(buffer_t *this);
    enum edit_result (*input_key)(buffer_t *this, u32 c);
    void (*enable_highlight)(buffer_t *this, uint start, uint end, int highlight_style);

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
