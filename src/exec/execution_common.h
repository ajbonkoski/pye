#ifndef EXECUTION_COMMON_H
#define EXECUTION_COMMON_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// helpful macros
#define PYE_EXT_TYPE_INIT(type) do {            \
    (type).tp_new = PyType_GenericNew;          \
    if (PyType_Ready(&(type)) < 0) return;      \
    Py_INCREF(&(type)); } while(0)

#define PYE_EXT_TYPE_NEW(ttype, itype)   \
    itype *self; do { \
    PyTypeObject *t = &(ttype); \
    self = (itype *)t->tp_alloc(t, 0); \
    if(self == NULL) { \
        DEBUG("Failed to make a " #itype  " object\n"); \
        return NULL; \
    } } while(0)

#ifdef __cplusplus
}
#endif

#endif
