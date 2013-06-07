#ifndef STRING_LIB_H
#define STRING_LIB_H

#include "common.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// backwards compantabile with c-strings
typedef struct
{
    size_t len;
    char *str;

} string_t;

string_t *string_alloc(size_t initialsize);
void      string_free(string_t *this);

// if 'this' is NULL, allocation memory. Returns 'this' or the allocated memory
string_t *string_copy(string_t *this, string_t *other);

// concats 'other' onto 'this', returning 'this'
string_t *string_concat(string_t *this, string_t *other);

inline bool string_equals(string_t *this, string_t *other)
{
    return strcmp(this->str, other->str) == 0 ? true : false;
}


#ifdef __cplusplus
}
#endif

#endif
