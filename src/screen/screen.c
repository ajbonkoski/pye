#include "screen.h"

#define IMPL_TYPE 0x7c870c4b

typedef struct
{
    screen_t *super;

} screen_internal_t;
screen_internal_t *cast_this(screen_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a screen_internal_t object");
    return (screen_internal_t *)s->impl;
}

static void destroy(screen_t *scrn)
{
    screen_internal_t *this = cast_this(scrn);

    free(this);
    free(scrn);
}

static screen_internal_t *screen_create_internal(screen_t *s)
{
    screen_internal_t *this = calloc(1, sizeof(screen_internal_t));
    this->super = s;

    return this;
}

screen_t *screen_create(void)
{
    screen_t *s = calloc(1, sizeof(screen_t));
    s->impl = screen_create_internal(s);
    s->impl_type = IMPL_TYPE;
    s->destroy = destroy;

    return s;
}
