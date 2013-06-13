#ifndef EXECUTION_SCREEN_H
#define EXECUTION_SCREEN_H

#include <Python.h>
#include "common/common.h"
#include "screen/screen.h"

#ifdef __cplusplus
extern "C" {
#endif

void execution_screen_init(void);
PyObject *execution_screen_create(screen_t *scrn);

#ifdef __cplusplus
}
#endif

#endif
