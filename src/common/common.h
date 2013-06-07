#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// define debugging and error macros
#define ENABLE_DEBUG 0
#define DEBUG(...) do { if(ENABLE_DEBUG) fprintf(stderr, __VA_ARGS__); } while(0)
#define ERROR(...) fprintf(stderr, __VA_ARGS__)

// better assert()
#define ASSERT(cond, msg) assert((cond) && msg)
#define ASSERT_UNIMPL() assert(0 && "Unimplemented");

// define booleans
#ifndef __cplusplus
typedef enum {
    true = 0,
    false = 1
} bool;
#endif

#ifdef __cplusplus
}
#endif

#endif
