#include "kill_buffer.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

struct kill_buffer
{
    size_t size;
    size_t used;
    size_t start; // the location of the 0th element
    void **data;   // array of elements (reated as a circular buffer)
    void (*destroyf)(void *);
};

// compute the "real" location
static inline size_t compute_index(kill_buffer_t *this, size_t i)
{
    return (i + this->start) % this->size;
}

kill_buffer_t *kill_buffer_create(size_t size, void (*destroyf)(void *))
{
    kill_buffer_t *this = calloc(1, sizeof(kill_buffer_t));
    this->size = size;
    this->used = 0;
    this->start = 0;
    this->data = malloc(this->size * sizeof(void *));
    this->destroyf = destroyf;

    return this;
}

void kill_buffer_destroy(kill_buffer_t *this)
{
    // delete each element
    for(size_t i = 0; i < this->size; i++) {
        void *elem = kill_buffer_get(this, i);
        this->destroyf(elem);
    }

    free(this->data);
    free(this);
}

void kill_buffer_set_max_size(kill_buffer_t *this, size_t size)
{
    // this function isn't entirely trivial:
    //   if size < this->size: the old elements need to be freed
    //   also, realloc() is not sufficient: we need to re-add each element.
    ASSERT_UNIMPL();
}

size_t kill_buffer_get_max_size(kill_buffer_t *this)
{
    return this->size;
}

size_t kill_buffer_get_size(kill_buffer_t *this)
{
    return this->used;
}

static void kill_buffer_forget1(kill_buffer_t *this)
{
    ASSERT(this->size == this->used, "forget1 called with size == used");
    size_t ri = compute_index(this, this->size-1);
    void *elem = this->data[ri];
    this->destroyf(elem);
    this->used -= 1;
}

// inserts 'obj' as the 0th element
void kill_buffer_add(kill_buffer_t *this, void *obj)
{
    if(this->size == this->used)
        kill_buffer_forget1(this);

    size_t ri = compute_index(this, this->size-1);
    this->data[ri] = obj;
    this->used++;
    this->start = ri;
}

// get's the ith element. Note: the 0th element is the one most recently added
void *kill_buffer_get(kill_buffer_t *this, size_t i)
{
    ASSERT(0 <= i && i < this->used, "index out-of-range in kill_buffer_get");
    size_t ri = compute_index(this, i);
    DEBUG("start=%zu, ri=%zu\n", this->start, ri);
    return this->data[ri];
}
