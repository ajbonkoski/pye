#ifndef EXECUTION_KEYBOARD_H
#define EXECUTION_KEYBOARD_H

#include <Python.h>
#include "structmember.h"

#include "common/common.h"
#include "display/keyboard.h"

#ifdef __cplusplus
extern "C" {
#endif

void execution_keyboard_init(void);
PyObject *execution_keyboard_create(void);

#ifdef __cplusplus
}
#endif

#endif
