#include "callable.h"

callable_t *callable_create(function_t f, void *usr, uint impl)
{
    return callable_init(malloc(sizeof(callable_t)), f, usr, impl);
}

callable_t *callable_init(callable_t *c, function_t f, void *usr, uint impl)
{
    memset(c, 0, sizeof(*c));
    c->func = f;
    c->usr = usr;
    c->impl = impl;
    return c;
}

callable_t *callable_create_c(function_t f, void *usr)
{
    return callable_init(malloc(sizeof(callable_t)), f, usr, CALLABLE_C_IMPL);
}

callable_t *callable_init_c(callable_t *c, function_t f, void *usr)
{
    return callable_init(c, f, usr, CALLABLE_C_IMPL);
}

callable_t *callable_copy(callable_t *orig)
{
    callable_t *ret = malloc(sizeof(callable_t));
    memcpy(ret, orig, sizeof(callable_t));
    return ret;
}

void callable_destroy(callable_t *this)
{
    callable_cleanup(this);
    free(this);
}

void callable_cleanup(callable_t *this)
{
    // do nothing currently
}
