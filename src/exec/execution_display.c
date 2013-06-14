#include "execution_display.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD
    display_t *display;
} pye_Display;

static PyObject *Display_clear(pye_Display *self)
{
    DEBUG("display.clear called\n");

    self->display->clear(self->display);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Display_set_cursor(PyObject *self, PyObject *args)
{
    uint x, y;
    if(!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;

    pye_Display *d = (pye_Display *)self;
    d->display->set_cursor(d->display, x, y);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Display_get_cursor(PyObject *self)
{
    uint x, y;
    pye_Display *d = (pye_Display *)self;
    d->display->get_cursor(d->display, &x, &y);

    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, PyInt_FromLong((long)x));
    PyTuple_SetItem(tuple, 1, PyInt_FromLong((long)y));
    return tuple;
}

static PyObject *Display_get_size(PyObject *self)
{
    uint w, h;
    pye_Display *d = (pye_Display *)self;
    d->display->get_size(d->display, &w, &h);

    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, PyInt_FromLong((long)w));
    PyTuple_SetItem(tuple, 1, PyInt_FromLong((long)h));
    return tuple;
}

static PyObject *Display_write(PyObject *self, PyObject *args)
{
    const char *str;
    size_t num;
    if(!PyArg_ParseTuple(args, "z#", &str, &num))
        return NULL;

    pye_Display *d = (pye_Display *)self;
    d->display->write(d->display, str, num, -1);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Display_methods[] = {
    {"clear", (PyCFunction)Display_clear, METH_NOARGS,
     "Clear the associated display."},
    {"set_cursor", (PyCFunction)Display_set_cursor, METH_VARARGS,
     "Set cursos on the associated display."},
    {"get_cursor", (PyCFunction)Display_get_cursor, METH_NOARGS,
     "Get the cursor location of the associated display."},
    {"get_size", (PyCFunction)Display_get_size, METH_NOARGS,
     "Get the current size of the associated display."},
    {"write", (PyCFunction)Display_write, METH_VARARGS,
     "Write data to the associated display from the current cursor position."},
    {NULL}  /* Sentinel */
};

static PyTypeObject pye_DisplayType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pye.Display",             /*tp_name*/
    sizeof(pye_Display),       /*tp_basicsize*/
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
    "Pye Display object",      /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Display_methods,           /* tp_methods */
};


void execution_display_init(void)
{
    pye_DisplayType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pye_DisplayType) < 0)
        return;

    Py_INCREF(&pye_DisplayType);
}

PyObject *execution_display_create(display_t *disp)
{
    PyTypeObject *t = &pye_DisplayType;
    pye_Display *self = (pye_Display *)t->tp_alloc(t, 0);
    if(self == NULL) {
        DEBUG("Failed to make a pye_Display object\n");
        return NULL;
    }

    self->display = disp;
    return (PyObject *)self;
}
