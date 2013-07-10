#ifndef EXECUTION_VARARGS_H
#define EXECUTION_VARARGS_H

#include <Python.h>
#include "common/common.h"
#include "common/varargs.h"

#ifdef __cplusplus
extern "C" {
#endif

// creates a new PyTuple Ref from the args
// User is responsible for DECREF
PyObject *execution_varargs_to_py(varargs_t *varargs);

// creates a new varags object from the pyObject
// User is responsible for freeing mem
varargs_t *execution_varargs_to_c(PyObject *args);
varargs_t *execution_varargs_to_c_skip(PyObject *args, uint skip);

#ifdef __cplusplus
}
#endif

#endif
