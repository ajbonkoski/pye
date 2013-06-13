#include "execution_screen.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD
    screen_t *screen;
} pye_Screen;

static PyObject *Screen_doit2(pye_Screen *self)
{
    DEBUG("screen.doit2 called\n");

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Screen_methods[] = {
    {"doit2", (PyCFunction)Screen_doit2, METH_NOARGS,
     "A testing function to make sure the plumbing is right."},
    {NULL}  /* Sentinel */
};

static PyTypeObject pye_ScreenType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pye.Screen",              /*tp_name*/
    sizeof(pye_Screen),        /*tp_basicsize*/
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
    "Pye Screen object",       /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Screen_methods,            /* tp_methods */
};


void execution_screen_init(void)
{
    pye_ScreenType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pye_ScreenType) < 0)
        return;

    Py_INCREF(&pye_ScreenType);
}

PyObject *execution_screen_create(screen_t *scrn)
{
    PyTypeObject *t = &pye_ScreenType;
    pye_Screen *self = (pye_Screen *)t->tp_alloc(t, 0);
    if(self == NULL) {
        DEBUG("Failed to make a pye_Screen object\n");
        return NULL;
    }

    self->screen = scrn;
    return (PyObject *)self;
}
