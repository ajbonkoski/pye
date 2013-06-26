#include "varray.h"

#define MIN_ALLOC 16

//#undef ENABLE_DEBUG
//#define ENABLE_DEBUG 1

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

void  varray_grow(varray_t *this, uint sz, void *val)
{
    ASSERT(this->size < sz, "sz must be greater than this->size in varray_grow()");

    while(this->size < sz)
        varray_add(this, val);
}

void  varray_shrink(varray_t *this, uint sz, void (*destroy)(void *))
{
    ASSERT(this->size > sz, "sz must be less than this->size in varray_shrink()");

    if(destroy != NULL) {
        for(uint i = sz; i < this->size; i++) {
            destroy(this->data[i]);
        }
    }

    this->size = sz;
}

void varray_add(varray_t *this, void *obj)
{
    DEBUG("varray_add(): Adding obj=0x%lx at i=%d\n", (ulong)obj, this->size);

    if(this->size == this->alloc) {
        DEBUG("varray_add(): reallocating...\n");
        this->alloc *= 2;
        this->data = realloc(this->data, this->alloc * sizeof(void *));
   }

    this->data[this->size++] = obj;
}

void *varray_get(varray_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "Error: Out of Bounds in varray_get()");
    DEBUG("varray_add(): getting now data[%d] == 0x%lx\n", i, (ulong)this->data[i]);
    return this->data[i];
}

void  varray_set(varray_t *this, uint i, void *obj)
{
    ASSERT(0 <= i && i < this->size, "Error: Out of Bounds in varray_set()");
    this->data[i] = obj;
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
        f(obj);
    }
}

void varray_destroy(varray_t *this)
{
    if(this->data != NULL)
        free(this->data);
    free(this);
}

