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

// forward declarations
static void add_buffer(screen_t *scrn, void *buffer);
static varray_t *list_buffers(screen_t *scrn);
static void set_buffer(screen_t *scrn, uint id);
static void write_mb(screen_t *scrn, const char *str);
static void destroy(screen_t *scrn);


static void add_buffer(screen_t *scrn, void *buffer)
{
    //screen_internal_t *this = cast_this(scrn);
    ASSERT_UNIMPL();
}

static varray_t *list_buffers(screen_t *scrn)
{
    //screen_internal_t *this = cast_this(scrn);
    ASSERT_UNIMPL();
    return NULL;
}

static void set_buffer(screen_t *scrn, uint id)
{
    //screen_internal_t *this = cast_this(scrn);
    ASSERT_UNIMPL();
}

static void write_mb(screen_t *scrn, const char *str)
{
    //screen_internal_t *this = cast_this(scrn);
    ASSERT_UNIMPL();
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

screen_t *screen_create(display_t *disp)
{
    screen_t *s = calloc(1, sizeof(screen_t));
    s->impl = screen_create_internal(s);
    s->impl_type = IMPL_TYPE;

    s->add_buffer = add_buffer;
    s->list_buffers = list_buffers;
    s->set_buffer = set_buffer;
    s->write_mb = write_mb;
    s->destroy = destroy;

    return s;
}
