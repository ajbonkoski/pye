#include "buffer.h"

// forward declarations
static void destroy(buffer_t *this);

static void destroy(buffer_t *this)
{
    free(this);
}

buffer_t *buffer_create(void)
{
    buffer_t *b = calloc(1, sizeof(buffer_t));
    b->destroy = destroy;

    return b;
}
