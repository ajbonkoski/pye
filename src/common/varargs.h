#ifndef VARARGS_H
#define VARARGS_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* a proper varargs object because the default
   va_list really sucks */

typedef struct varargs varargs_t;

varargs_t *varargs_create(void);
varargs_t *varargs_create_v(uint num, ...);
void varargs_destroy(varargs_t *this);

void varargs_add(varargs_t *this, char type_ch, void *data);
void *varargs_get(varargs_t *this, uint i);
char varargs_get_type(varargs_t *this, uint i);
uint varargs_size(varargs_t *this);

/*******  Char Types *******
 * s:   char *
 * i:   long
 * v:   void *
 * f:   func
 **************************/

#ifdef __cplusplus
}
#endif

#endif
