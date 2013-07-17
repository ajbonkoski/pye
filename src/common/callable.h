#ifndef CALLABLE_H
#define CALLABLE_H

#include "common.h"
#include "varargs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This object provides a clean object-like interface to function/method calls
   This abstraction is crutial for provinding a uniform c-to-scripting interop */

#define CALLABLE_C_IMPL 0x7d3bb0d4

typedef void (*generic_function_t)(void *this, varargs_t *va);
typedef struct callable
{
    uint impl;  // where does this callable stem from? C? Scripting? Other?
    generic_function_t func;
    void *usr;

} callable_t;

callable_t *callable_create(generic_function_t f, void *usr, uint impl); // allocates on heap
callable_t *callable_init(callable_t *c, generic_function_t f, void *usr, uint impl); // initializes struct on stack

// make a copy - return heap-allocated memory
callable_t *callable_copy(callable_t *orig);

callable_t *callable_create_c(generic_function_t f, void *usr); // allocates on heap
callable_t *callable_init_c(callable_t *c, generic_function_t f, void *usr); // initializes struct on stack

void callable_destroy(callable_t *this); // cleans up heap-based memory
void callable_cleanup(callable_t *this); // cleans up stack-based memory

#define callable_call(this, ...) \
    do { this->func(this->usr, __VA_ARGS__); } while(0)


#ifdef __cplusplus
}
#endif

#endif
