#ifndef EXECUTION_BUFFER_H
#define EXECUTION_BUFFER_H

#include <Python.h>
#include "common/common.h"
#include "buffer/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

void execution_buffer_init(void);
PyObject *execution_buffer_create(buffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif
