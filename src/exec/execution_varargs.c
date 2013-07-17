#include "execution_varargs.h"
#include "common/callable.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define CALLABLE_PY_IMPL 0x2bac5de3

PyObject *python_wrapped_c_callback(PyObject *obj, PyObject *args)
{
    callable_t *c = (callable_t *)obj;
    varargs_t *va = execution_varargs_to_c(args);
    callable_call(c, va);
    varargs_destroy(va);

    Py_INCREF(Py_None);
    return Py_None;
}

void c_wrapped_python_callback(void *this, varargs_t *va)
{
    PyObject *func = (PyObject *)this;
    ASSERT(PyCallable_Check(func), "func must be callable in c_wrapped_python_callback");
    PyObject *args = execution_varargs_to_py(va);
    DEBUG("BEFORE call to python in c_wrapped_python_callback\n");
    PyObject *result = PyObject_Call(func, args, NULL);
    DEBUG("AFTER call to python in c_wrapped_python_callback\n");

    // cleanup
    Py_DECREF(args);
    Py_XDECREF(result);
    DEBUG("AFTER cleanup in c_wrapped_python_callback\n");
}

PyObject *convert_callable_to_py(callable_t *c)
{
    switch(c->impl) {
        case CALLABLE_C_IMPL: {
            struct PyMethodDef def = {
                "python_wrapped_c_callback", /* ml_name  */
                python_wrapped_c_callback,   /* ml_meth  */
                0,                           /* ml_flags */
                "A python function object wrapping a C callback", /* ml_doc   */
            };
            PyObject *f = PyCFunction_New(&def, (PyObject *)c);
            Py_INCREF(f);
            return f;
        }
        case CALLABLE_PY_IMPL: {
            PyObject *f = (PyObject *)c->usr;
            Py_INCREF(f);
            return f;
        }
        default:
            ASSERT_FAIL("failed to parse the source of the callable\n");

    }

    return NULL;
}

PyObject *execution_varargs_to_py(varargs_t *va)
{
    ASSERT(va != NULL, "varargs must be non-null");
    uint size = varargs_size(va);
    PyObject *tuple = PyTuple_New(size);

    for(uint i = 0; i < size; i++) {

        char datatype = varargs_get_type(va, i);
        PyObject *obj = NULL;
        switch(datatype) {
            case 's': obj = PyString_FromString(varargs_get(va, i));    break;
            case 'i': obj = PyInt_FromLong((long)varargs_get(va, i));   break;
            case 'c': obj = convert_callable_to_py(varargs_get(va, i)); break;
            case 'o': obj = (PyObject *)varargs_get(va, i);           break;
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
            Py_INCREF(obj);  /* keep python from destroying this function object from under us! */
            cobj = callable_create(c_wrapped_python_callback, (void *)obj, CALLABLE_PY_IMPL);
            ctype = 'c';
        } else { // assume its a PyObject
            cobj = (void *)obj;
            ctype = 'o';
        }

        varargs_add(va, ctype, cobj);
    }

    return va;
}
