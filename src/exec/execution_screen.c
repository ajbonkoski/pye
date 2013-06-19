#include "execution_screen.h"
#include "execution_buffer.h"

//#undef ENABLE_DEBUG
//#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD
    screen_t *screen;
    PyObject *key_func; // XXX: this is not cleaned up properly
    PyObject *buf_func; // XXX: this is not cleaned up properly
} pye_Screen;

static PyObject *Screen_mb_write(pye_Screen *self, PyObject *args)
{
    const char *str;
    if(!PyArg_ParseTuple(args, "z", &str))
        return NULL;

    self->screen->mb_write(self->screen, str);

    Py_INCREF(Py_None);
    return Py_None;
}

static bool screen_key_event_handler(void *usr, key_event_t *e)
{
    PyObject *func = (PyObject *)usr;
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyInt_FromLong(e->key_code));
    PyObject *result = PyObject_CallObject(func, args);

    bool ret = false;
    if(!PyBool_Check(result)) {
        ERROR("python key handler did not return a boolean\n");
        ret = false;
    } else if(result == Py_True) {
        ret = true;
    } else if(result == Py_False) {
        ret = false;
    } else {
        ASSERT_FAIL("screen_key_event_handler: this should never happen...");
    }

    // some cleanup
    Py_DECREF(result);
    Py_DECREF(args);

    return ret;
}

static void screen_buf_event_handler(void *usr, uint index)
{
    PyObject *func = (PyObject *)usr;
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyInt_FromLong(index));
    PyObject *result = PyObject_CallObject(func, args);

    // cleanup
    Py_XDECREF(result);
    Py_DECREF(args);
}

static PyObject *Screen_register_kbrd_callback(pye_Screen *self, PyObject *args)
{
    DEBUG("inside Screen_register_kbrd_callback\n");

    PyObject *func = NULL;
    if(!PyArg_ParseTuple(args, "O", &func))
        return NULL;

    if(!PyCallable_Check(func)) {
        ERROR("error: Screen_register_kbrd_callback recieved uncallable parameter\n");
        Py_XDECREF(func);
        goto ret;
    }

    screen_t *s = self->screen;
    s->register_kbrd_callback(s, screen_key_event_handler, (void *)func);
    self->key_func = func;

 ret:
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Screen_register_buf_callback(pye_Screen *self, PyObject *args)
{
    DEBUG("inside Screen_register_buf_callback\n");

    PyObject *func = NULL;
    if(!PyArg_ParseTuple(args, "O", &func))
        return NULL;

    if(!PyCallable_Check(func)) {
        ERROR("error: Screen_register_buf_callback recieved uncallable parameter\n");
        Py_XDECREF(func);
        goto ret;
    }

    screen_t *s = self->screen;
    s->register_buf_added_callback(s, screen_buf_event_handler, (void *)func);
    self->buf_func = func;

 ret:
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Screen_get_buffer(pye_Screen *self, PyObject *args)
{
    uint i;
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

    buffer_t *b = self->screen->get_buffer(self->screen, i);
    PyObject *pBuffer = execution_buffer_create(b);
    return pBuffer;
}

static PyObject *Screen_get_active_buffer(pye_Screen *self)
{
    buffer_t *b = self->screen->get_active_buffer(self->screen);
    if(b == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject *pBuffer = execution_buffer_create(b);
    return pBuffer;
}

static PyObject *Screen_refresh(pye_Screen *self)
{
    self->screen->refresh(self->screen);
    Py_INCREF(Py_None);
    return Py_None;
}

// this should be in buffer_t (API is broken)
static PyObject *Screen_get_viewport_line(pye_Screen *self)
{
    uint vpy = self->screen->get_viewport_line(self->screen);
    return PyInt_FromLong(vpy);
}

static PyMethodDef Screen_methods[] = {
    {"mb_write", (PyCFunction)Screen_mb_write, METH_VARARGS,
     "Write a message to the screen's Message buffer."},
    {"on_key", (PyCFunction)Screen_register_kbrd_callback, METH_VARARGS,
     "Register a key event handler for the screen object."},
    {"on_buffer_added", (PyCFunction)Screen_register_buf_callback, METH_VARARGS,
     "Register a buffer added event handler for the screen object."},
    {"get_buffer", (PyCFunction)Screen_get_buffer, METH_VARARGS,
     "Get a buffer by its integer id number."},
    {"get_active_buffer", (PyCFunction)Screen_get_active_buffer, METH_NOARGS,
     "Get the Screen's active Buffer."},
    {"refresh", (PyCFunction)Screen_refresh, METH_NOARGS,
     "Redraw the screen on the display."},
    {"get_viewport_line", (PyCFunction)Screen_get_viewport_line, METH_NOARGS,
     "Get the line number at the top of the screen (Note: this should really be a method of pye.Buffer)."},
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
