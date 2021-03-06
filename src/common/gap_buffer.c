#include "gap_buffer.h"

#define INITIAL_ALLOC 5

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

struct gap_buffer
{
    size_t size;      // number of characters actually used
    size_t alloc;     // how many char are allocated
    size_t gap_start; // index of where the gap begins
    size_t elemsize;
    u8 *data;
};

#define gap_size() (this->alloc - this->size)


gap_buffer_t *gap_buffer_create(size_t elemsize)
{
    gap_buffer_t *this = calloc(1, sizeof(gap_buffer_t));
    this->size = 0;
    this->alloc = INITIAL_ALLOC;
    this->gap_start = 0;
    this->elemsize = elemsize;
    this->data = malloc(this->alloc * this->elemsize);

    return this;
}

gap_buffer_t *gap_buffer_create_from_str(const u8 *str, size_t numelts, size_t elemsize)
{
    gap_buffer_t *this = gap_buffer_create(elemsize);
    for(int i = 0; i < numelts; i++) {
        gap_buffer_insert(this, i, ((u8 *)str) + i*elemsize);
    }

    return this;
}

void gap_buffer_destroy(gap_buffer_t *this)
{
    ASSERT(this != NULL, "tried to destroy a NULL gap buffer");
    if(this->data) free(this->data);
    free(this);
}

static void reallocate_buffer(gap_buffer_t *this)
{
    // alias's
    size_t es = this->elemsize;

    // allocate
    size_t nalloc = 2*this->alloc;
    u8 *ndata = malloc(nalloc * this->elemsize);

    // copy first half
    for(size_t i = 0; i < this->gap_start * es; i += es) {
        memcpy(ndata+i, this->data+i, es);
    }

    // compute locations
    size_t end = this->gap_start + gap_size();
    size_t endsize = this->alloc - end;
    size_t newend = nalloc - endsize;

    // copy last half
    u8 *ptr = this->data + end * this->elemsize;
    u8 *nptr = ndata + newend * this->elemsize;
    for(size_t i = 0; i < endsize * es; i += es) {
        memcpy(nptr+i, ptr+i, es);
    }

    // update the gap_buffer struct
    this->alloc = nalloc;
    free(this->data);
    this->data = ndata;
}

void gap_buffer_set_focus(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i <= this->size, "index out-of-bounds in 'gap_buffer_set_focus'");
    size_t es = this->elemsize;

    size_t gapsz = gap_size();
    if(gapsz <= 0) {
        this->gap_start = i;
        reallocate_buffer(this);
        return;
    }

    // if already there, save some work
    if(i == this->gap_start)
        return;

    size_t end = this->gap_start + gapsz;

    {
        u8 *p1 = this->data + end * es;
        u8 *p2 = this->data + this->gap_start * es;
        while(i < this->gap_start) {
            p1-=es;
            p2-=es;
            --this->gap_start;
            memcpy(p1, p2, es);
        }
    }

    {
        u8 *p1 = this->data + this->gap_start * es;
        u8 *p2 = this->data + end * es;
        while(i > this->gap_start) {
            memcpy(p1, p2, es);
            p1+=es;
            p2+=es;
            ++this->gap_start;
        }
    }

}

size_t gap_buffer_size(gap_buffer_t *this)
{
    return this->size;
}

void *gap_buffer_get(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_get'");
    size_t es = this->elemsize;

    if(i < this->gap_start)
        return this->data + i*es;
    else
        return this->data + (i + gap_size()) * es;
}

void *gap_buffer_to_str(gap_buffer_t *this, void *databuf)
{
    size_t es = this->elemsize;
    size_t sz = gap_buffer_size(this);
    void *str = databuf == NULL ? malloc((sz+1)*es) : databuf;
    for(size_t i = 0; i < sz; i++) {
        void *val = gap_buffer_get(this, i);
        memcpy(str + i*es, val, es);
    }
    memset(str + sz*es, 0, es);
    return str;
}

void gap_buffer_insert(gap_buffer_t *this, uint i, void *c)
{
    ASSERT(0 <= i && i <= this->size, "index out-of-bounds in 'gap_buffer_insert'");
    gap_buffer_set_focus(this, i);
    ASSERT(this->alloc != this->size, "");
    size_t es = this->elemsize;
    size_t gs = this->gap_start;
    memcpy(this->data + gs*es, c, es);
    this->gap_start++;
    this->size++;
}

// delete the char the cursor is on
void gap_buffer_delr(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_delr'");

    gap_buffer_set_focus(this, i);
    this->size--;
}

// delete the char to the left of the cursor
void gap_buffer_dell(gap_buffer_t *this, uint i)
{
    ASSERT(0 < i && i <= this->size, "index out-of-bounds in 'gap_buffer_dell'");

    gap_buffer_set_focus(this, i);
    this->gap_start--;
    this->size--;
}

// splits 'this' at location i and returns a new gap_buffer with the data
gap_buffer_t *gap_buffer_split(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i <= this->size, "index out-of-bounds in 'gap_buffer_split'");
    gap_buffer_set_focus(this, i);
    size_t end = this->gap_start + gap_size();
    size_t endsize = this->alloc - end;

    size_t es = this->elemsize;
    gap_buffer_t *gb = gap_buffer_create_from_str(this->data + end*es, endsize, es);
    this->size = this->gap_start;
    return gb;
}

// merge 'other' onto the end of 'this' and free 'other'
void gap_buffer_join(gap_buffer_t *this, gap_buffer_t *other)
{
    // this is an expensive implementation... TODO: optimize

    // alias's
    uint es = this->elemsize;

    // get the total size
    uint this_size = this->size;
    uint other_size = other->size;
    uint total_size = this_size + other_size;

    // get a string of the data
    u8 *this_str = gap_buffer_to_str(this, NULL);
    u8 *other_str = gap_buffer_to_str(other, NULL);
    u8 *total_str = malloc((total_size+1) * es);
    memcpy(total_str, this_str, this_size * es);
    memcpy(total_str + this_size * es, other_str, other_size * es);
    memset(total_str + total_size * es, 0, es);

    // some quick cleanup before moving on
    free(this_str);
    free(other_str);
    gap_buffer_destroy(other);

    // reset 'this' internal state
    this->size = 0;
    this->gap_start = 0;

    // insert each element
    u8 *ptr = total_str;
    for(uint i = 0; i < total_size; i++) {
        gap_buffer_insert(this, i, ptr);
        ptr += es;
    }

    // final cleanup
    free(total_str);
}
