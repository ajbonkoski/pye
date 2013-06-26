#ifndef VARRAY_H
#define VARRAY_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct varray varray_t;

varray_t *varray_create();

// if this->size > sz: chop off the end of the array
// otherwise: add 'val' to the end until this->size == sz
void  varray_grow(varray_t *this, uint sz, void *val);
void  varray_shrink(varray_t *this, uint sz, void (*destroy)(void *));
void  varray_add(varray_t *this, void *obj);
void *varray_get(varray_t *this, uint i);
void  varray_set(varray_t *this, uint i, void *obj);
uint  varray_size(varray_t *this);
bool  varray_iter_next(varray_t *this, int *it, void **val);
void  varray_map(varray_t *this, void (*f)(void *));
void  varray_destroy(varray_t *this);

// helper macros
#define varray_iter(variable, varray) \
    for(int varray_iter_variable = 0; varray_iter_next((varray), &varray_iter_variable, (void **)&(variable));)

#ifdef __cplusplus
}
#endif

#endif
