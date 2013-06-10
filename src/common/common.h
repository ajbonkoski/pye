#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// define booleans
#ifndef __cplusplus
typedef enum {
    false = 0,
    true = 1
} bool;
#endif

// some integer typedefs
typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;

// define debugging and error macros
#define ENABLE_DEBUG 0
FILE *common_get_debug_file(void);
#define DEBUG(...) do { if(ENABLE_DEBUG) { \
    FILE *debug_file = common_get_debug_file(); \
    fprintf(debug_file, __VA_ARGS__); } } while(0)
#define ERROR(...) do { \
    FILE *debug_file = common_get_debug_file(); \
    fprintf(debug_file, __VA_ARGS__); } while(0)

// better assert()
typedef void (*crash_func_t)(void *);
void set_crash_func(crash_func_t crash_func, void *usr);
bool do_crash();
#define ASSERT(cond, msg) assert(((cond) && msg) || do_crash())
#define ASSERT_FAIL(msg) assert((0 && msg) || do_crash())
#define ASSERT_UNIMPL() assert((0 && "Unimplemented") || do_crash());

#ifdef __cplusplus
}
#endif

#endif
