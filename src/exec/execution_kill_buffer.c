#include "execution_kill_buffer.h"
#include "execution_common.h"
#include "common/strsafe.h"

/* #undef ENABLE_DEBUG */
/* #define ENABLE_DEBUG 1 */

typedef struct {
    PyObject_HEAD
    kill_buffer_t *kb;

} pye_KillBuffer;

static PyObject *KillBuffer_set_max_size(pye_KillBuffer *self, PyObject *args)
{
    size_t i;
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

    kill_buffer_set_max_size(self->kb, i);

    Py_RETURN_NONE;
}

static PyObject *KillBuffer_get_size(pye_KillBuffer *self)
{
    size_t sz = kill_buffer_get_size(self->kb);
    return PyInt_FromLong(sz);
}

static PyObject *KillBuffer_add(pye_KillBuffer *self, PyObject *args)
{
    const char *s;
    uint num;
    if(!PyArg_ParseTuple(args, "s#", &s, &num))
        return NULL;

    strsafe_t *ss = strsafe_create(num);
    strsafe_cat_cstr(ss, s, num);
    kill_buffer_add(self->kb, ss);

    Py_RETURN_NONE;
}

static PyObject *KillBuffer_get(pye_KillBuffer *self, PyObject *args)
{
    uint i;
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

    strsafe_t *ss = kill_buffer_get(self->kb, i);
    return PyString_FromStringAndSize(ss->data, ss->len);
}


static PyMethodDef KillBuffer_methods[] = {
    {"set_max_size", (PyCFunction)KillBuffer_set_max_size, METH_VARARGS,
     "Set the maximum history size of the kill buffer."},
    {"get_size", (PyCFunction)KillBuffer_get_size, METH_NOARGS,
     "Gte the current size of the kill buffer history."},
    {"add", (PyCFunction)KillBuffer_add, METH_VARARGS,
     "Add a string to the kill buffer. The added string is placed at the beginning."},
    {"get", (PyCFunction)KillBuffer_get, METH_VARARGS,
     "Get data from the kill buffer. Lower indicies indicate data added most recently."},
    {NULL}  /* Sentinel */
};

static PyMemberDef KillBuffer_members[] = {
    {NULL}  /* Sentinel */
};

static PyTypeObject pye_KillBufferType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pye.KillBuffer",          /*tp_name*/
    sizeof(pye_KillBuffer),    /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Pye KillBuffer object",   /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    KillBuffer_methods,        /* tp_methods */
    KillBuffer_members,        /* tp_members */
};


void execution_kill_buffer_init(void)
{
    PYE_EXT_TYPE_INIT(pye_KillBufferType);
}

PyObject *execution_kill_buffer_create(kill_buffer_t *kb)
{
    PYE_EXT_TYPE_NEW(pye_KillBufferType, pye_KillBuffer);
    self->kb = kb;

    return (PyObject *)self;
}
