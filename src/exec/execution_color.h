#ifndef EXECUTION_COLOR_H
#define EXECUTION_COLOR_H

#include <Python.h>
#include "structmember.h"

#include "common/common.h"
#include "display/display.h"

#ifdef __cplusplus
extern "C" {
#endif

void execution_color_init(void);
PyObject *execution_color_create(void);

#ifdef __cplusplus
}
#endif

#endif
