#ifndef EXECUTION_DISPLAY_H
#define EXECUTION_DISPLAY_H

#include <Python.h>
#include "common/common.h"
#include "display/display.h"

#ifdef __cplusplus
extern "C" {
#endif

void execution_display_init(void);
PyObject *execution_display_create(display_t *disp);

#ifdef __cplusplus
}
#endif

#endif
