#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// concat returns malloc'd memory, user is responsible for the cleanup
#define string_util_concat(...) __string_util_concat_varargs_private(__VA_ARGS__, NULL)
char *__string_util_concat_varargs_private(const char *first, ...) __attribute__((sentinel));

#ifdef __cplusplus
}
#endif

#endif
