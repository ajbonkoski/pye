#ifndef VARRAY_H
#define VARRAY_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct varray varray_t;

varray_t *varray_create();
void  varray_add(varray_t *this, void *obj);
void *varray_get(varray_t *this, uint i);
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
