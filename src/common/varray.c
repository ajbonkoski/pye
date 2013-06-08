#include "varray.h"

#define MIN_ALLOC 16

struct varray
{
    uint alloc;     // size of data[] in terms of allocated space
    uint size;      // size of data[] in terms of used space
    void **data;
};

varray_t *varray_create()
{
    varray_t *this = calloc(1, sizeof(varray_t));
    this->alloc = MIN_ALLOC;
    this->size = 0;
    this->data = malloc(this->alloc * sizeof(void *));
    return this;
}

void varray_add(varray_t *this, void *obj)
{
    if(this->size == this->alloc) {
        this->alloc *= 2;
        this->data = realloc(this->data, this->alloc);
    }

    this->data[this->size++] = obj;
}

void *varray_get(varray_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "Error: Out of Bounds in varray_get()");
    return this->data[i];
}

uint varray_size(varray_t *this)
{
    return this->size;
}

bool varray_iter_next(varray_t *this, int *it, void **val)
{
    // reached the end?
    if(*it >= this->size)
        return false;

    *val = this->data[*it];
    (*it)++;
    return true;
}

void varray_map(varray_t *this, void (*f)(void *))
{
    void *obj;
    varray_iter(obj, this) {
        f(this);
    }
}

void varray_destroy(varray_t *this)
{
    if(this->data != NULL)
        free(this->data);
    free(this);
}
