#ifndef KILL_BUFFER_H
#define KILL_BUFFER_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kill_buffer kill_buffer_t;

// a destroy() function for the elements must be provided
kill_buffer_t *kill_buffer_create(size_t size, void (*destroyf)(void *));
void kill_buffer_destroy(kill_buffer_t *this);

void kill_buffer_set_max_size(kill_buffer_t *this, size_t size);
size_t kill_buffer_get_max_size(kill_buffer_t *this);
size_t kill_buffer_get_size(kill_buffer_t *this);

// inserts 'obj' as the 0th element
void kill_buffer_add(kill_buffer_t *this, void *obj);

// get's the ith element. Note: the 0th element is the one most recently added
void *kill_buffer_get(kill_buffer_t *this, size_t i);

#ifdef __cplusplus
}
#endif

#endif
