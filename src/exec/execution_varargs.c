#include "execution_varargs.h"

PyObject *execution_varargs_to_py(varargs_t *va)
{
    ASSERT(va != NULL, "varargs must be non-null");
    uint size = varargs_size(va);
    PyObject *tuple = PyTuple_New(size);

    for(uint i = 0; i < size; i++) {

        char datatype = varargs_get_type(va, i);
        PyObject *obj = NULL;
        switch(datatype) {
            case 's': obj = PyString_FromString(varargs_get(va, i));  break;
            case 'i': obj = PyInt_FromLong((long)varargs_get(va, i));       break;
            case 'v': obj = (PyObject *)varargs_get(va, i);           break;
            case 'f': obj = (PyObject *)varargs_get(va, i);           break;
            default:
                ERROR("Unrecognized type character: %c\n", datatype);
                goto ret;
        }

        PyTuple_SetItem(tuple, i, obj);
    }

 ret:
    return tuple;
}

varargs_t *execution_varargs_to_c(PyObject *args)
{
    return execution_varargs_to_c_skip(args, 0);
}

varargs_t *execution_varargs_to_c_skip(PyObject *args, uint skip)
{
    ASSERT(PyTuple_Check(args), "args must be a python tuple");

    varargs_t *va = varargs_create();

    uint size = PyTuple_Size(args);
    ASSERT(skip <= size, "skip must be less than or equal to size");
    for(uint i = skip; i < size; i++) {

        PyObject *obj = PyTuple_GET_ITEM(args, i);

        void *cobj = NULL;
        char ctype;

        if(PyString_Check(obj)) {
            cobj = PyString_AsString(obj);
            ctype = 's';
        } else if(PyInt_Check(obj)) {
            cobj = (void *)PyInt_AsLong(obj);
            ctype = 'i';
        } else if(PyCallable_Check(obj)) {
            cobj = (void *)obj;
            ctype = 'f';
        } else {
            ERROR("Failed to get the type in execution_varargs_to_c\n");
            goto ret;
        }

        varargs_add(va, ctype, cobj);
    }

 ret:
    return va;
}
