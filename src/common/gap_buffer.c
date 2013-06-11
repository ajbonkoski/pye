#include "gap_buffer.h"

#define INITIAL_ALLOC 15

struct gap_buffer
{
    size_t size;      // number of characters actually used
    size_t alloc;     // how many char are allocated
    size_t gap_start; // index of where the gap begins
    char *data;
};

#define gap_size() (this->alloc - this->size)


gap_buffer_t *gap_buffer_create(void)
{
    gap_buffer_t *this = calloc(1, sizeof(gap_buffer_t));
    this->size = 0;
    this->alloc = INITIAL_ALLOC;
    this->gap_start = 0;
    this->data = malloc(this->alloc * sizeof(char));

    return this;
}

gap_buffer_t *gap_buffer_create_from_str(const char *str, size_t n)
{
    ASSERT_UNIMPL();
    return NULL;
}

void gap_buffer_destroy(gap_buffer_t *this)
{
    ASSERT(this != NULL, "tried to destroy a NULL gap buffer");
    if(this->data) free(this->data);
    free(this);
}

static void reallocate_buffer(gap_buffer_t *this)
{
    // allocate
    size_t nalloc = 2*this->alloc;
    char *ndata = malloc(nalloc*sizeof(char));

    // copy first half
    for(size_t i = 0; i < this->gap_start; i++) {
        ndata[i] = this->data[i];
    }

    // compute locations
    size_t end = this->gap_start + gap_size();
    size_t endsize = this->alloc - end;
    size_t newend = nalloc - endsize;

    // copy last half
    char *ptr = this->data + end;
    char *nptr = ndata + newend;
    for(size_t i = 0; i < endsize; i++) {
        nptr[i] = ptr[i];
    }

    // update the gap_buffer struct
    this->alloc = nalloc;
    free(this->data);
    this->data = ndata;
}

void gap_buffer_set_focus(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_set_focus'");

    // if already there, save some work
    if(i == this->gap_start)
        return;

    size_t gapsz = gap_size();
    if(gapsz <= 0) {
        this->gap_start = i;
        reallocate_buffer(this);
        return;
    }

    size_t end = this->gap_start + gapsz;

    while(i < this->gap_start) {
        this->data[--end] = this->data[--this->gap_start];
    }

    while(i > this->gap_start) {
        this->data[++this->gap_start] = this->data[++end];
    }
}

size_t gap_buffer_size(gap_buffer_t *this)
{
    return this->size;
}

char gap_buffer_get(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_get'");
    if(i < this->gap_start)
        return this->data[i];

    return this->data[i + gap_size()];
}

void gap_buffer_insert(gap_buffer_t *this, uint i, char c)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_insert'");
    gap_buffer_set_focus(this, i);
    this->data[this->gap_start++] = c;
    this->size++;
}

// delete the char the cursor is on
void gap_buffer_delr(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_delr'");
    if(i == this->size-1) {
        DEBUG("WRN: i=size-1 in gap_buffer_delr\n");
        return;
    }

    gap_buffer_set_focus(this, i);
    this->size--;
}

// delete the char to the left of the cursor
void gap_buffer_dell(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_dell'");
    if(i == 0) {
        DEBUG("WRN: i=0 in gap_buffer_dell\n");
        return;
    }

    gap_buffer_set_focus(this, i);
    this->gap_start--;
    this->size--;
}

// splits 'this' at location i and returns a new gap_buffer with the data
gap_buffer_t *gap_buffer_split(gap_buffer_t *this, uint i)
{
    ASSERT(0 <= i && i < this->size, "index out-of-bounds in 'gap_buffer_split'");
    gap_buffer_set_focus(this, i);
    size_t end = this->gap_start + gap_size();
    size_t endsize = this->alloc - end;

    gap_buffer_t *gb = gap_buffer_create_from_str(this->data + end, endsize);
    this->size = this->gap_start;
    return gb;
}

// merge 'other' onto the end of 'this' and free 'other'
void gap_buffer_join(gap_buffer_t *this, gap_buffer_t *other)
{
    ASSERT_UNIMPL();
}
