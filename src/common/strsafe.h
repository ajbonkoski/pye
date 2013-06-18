#ifndef STRSAFE_H
#define STRSAFE_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char *data;
    uint len;
    uint alloc; // not including the NULL

} strsafe_t;

// for stack allocation. Returns ss
strsafe_t *strsafe_init(strsafe_t *ss, uint initial_alloc);
void strsafe_cleanup(strsafe_t *ss);

// heap allocation
strsafe_t *strsafe_create(uint initial_alloc);
void strsafe_destroy(strsafe_t *ss);

// require that string 'ss' has enough space allocated to hold 'len' chars
// realloc() is used if necessary
void strsafe_require(strsafe_t *ss, uint len);

// perform string concatenation by conacatenating 'ssc' onto 'ss'.
// reallocation is performed if necessary
void strsafe_cat(strsafe_t *ss, strsafe_t *ssc);

void strsafe_cat_cstr(strsafe_t *ss, char *cstr, uint len);
void strsafe_cat_char(strsafe_t *ss, char c);

#ifdef __cplusplus
}
#endif

#endif
