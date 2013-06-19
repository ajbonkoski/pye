#ifndef EXECUTION_KILL_BUFFER_H
#define EXECUTION_KILL_BUFFER_H

#include <Python.h>
#include "structmember.h"

#include "common/common.h"
#include "screen/kill_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

void execution_kill_buffer_init(void);
PyObject *execution_kill_buffer_create(kill_buffer_t *kb);

#ifdef __cplusplus
}
#endif

#endif
