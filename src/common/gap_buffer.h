#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gap_buffer gap_buffer_t;

gap_buffer_t *gap_buffer_create(size_t elemsize);
gap_buffer_t *gap_buffer_create_from_str(const u8 *str, size_t numelts, size_t elemsize);
void gap_buffer_destroy(gap_buffer_t *this);

void gap_buffer_set_focus(gap_buffer_t *this, uint i);
size_t gap_buffer_size(gap_buffer_t *this);
void *gap_buffer_get(gap_buffer_t *this, uint i);

// 'databuf' allow the user to pass memory in. This memory MUST be correctly sized. If NULL is passed, the method allocates new memory and the user is responsible for freeing it
void *gap_buffer_to_str(gap_buffer_t *this, void *databuf);
void gap_buffer_insert(gap_buffer_t *this, uint i, void *c);
void gap_buffer_delr(gap_buffer_t *this, uint i);
void gap_buffer_dell(gap_buffer_t *this, uint i);

// splits 'this' at location i and returns a new gap_buffer with the data
gap_buffer_t *gap_buffer_split(gap_buffer_t *this, uint i);

// merge 'other' onto the end of 'this' and free 'other'
void gap_buffer_join(gap_buffer_t *this, gap_buffer_t *other);

#ifdef __cplusplus
}
#endif

#endif
