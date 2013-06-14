#include "execution_keyboard.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD
} pye_Keyboard;

static PyMethodDef Keyboard_methods[] = {
    {NULL}  /* Sentinel */
};

static PyTypeObject pye_KeyboardType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pye.Keyboard",            /*tp_name*/
    sizeof(pye_Keyboard),      /*tp_basicsize*/
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
    "Pye Keyboard object",     /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Keyboard_methods,          /* tp_methods */
};


void execution_keyboard_init(void)
{
    pye_KeyboardType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pye_KeyboardType) < 0)
        return;

    Py_INCREF(&pye_KeyboardType);
}

PyObject *execution_keyboard_create(void)
{
    PyTypeObject *t = &pye_KeyboardType;
    pye_Keyboard *self = (pye_Keyboard *)t->tp_alloc(t, 0);
    if(self == NULL) {
        DEBUG("Failed to make a pye_Keyboard object\n");
        return NULL;
    }

    return (PyObject *)self;
}
