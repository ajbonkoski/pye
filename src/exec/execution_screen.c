#include "execution_screen.h"
#include "execution_buffer.h"
#include "execution_varargs.h"
#include "mode/edit_mode.h"
#include "common/varargs.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define EDIT_MODE_PYTHON_IMPL 0xe51ecb06

typedef struct {
    PyObject_HEAD
    screen_t *screen;
    PyObject *key_func; // XXX: this is not cleaned up properly
    PyObject *buf_func; // XXX: this is not cleaned up properly
} pye_Screen;

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

static PyObject *Screen_list_buffers(pye_Screen *self)
{
    varray_t *va_buffers = self->screen->list_buffers(self->screen);
    PyObject *ret = PyList_New(varray_size(va_buffers));

    int i = 0;
    buffer_t *buf;
    varray_iter(buf, va_buffers) {
        PyObject *py_buf = execution_buffer_create(buf);
        PyList_SET_ITEM(ret, i, py_buf);
        i++;
    }

    return ret;
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

static PyObject *Screen_set_active_buffer(pye_Screen *self, PyObject *args)
{
    uint i;
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

    self->screen->set_active_buffer(self->screen, i);

    Py_INCREF(Py_None);
    return Py_None;
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

// this should be in buffer_t (API is broken)
static PyObject *Screen_center_viewport(pye_Screen *self)
{
    self->screen->center_viewport(self->screen);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Screen_mb_write(pye_Screen *self, PyObject *args)
{
    const char *str;
    if(!PyArg_ParseTuple(args, "z", &str))
        return NULL;

    self->screen->mb_write(self->screen, str);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Screen_mb_get_yloc(pye_Screen *self)
{
    uint yloc = self->screen->mb_get_yloc(self->screen);
    return PyInt_FromLong(yloc);
}

static void edit_mode_python_begin_mode(edit_mode_t *em, varargs_t *va)
{
    ASSERT(em->impl_type == EDIT_MODE_PYTHON_IMPL, "wrong impl");
    PyObject *obj = (PyObject *)em->impl;

    PyObject *meth = PyObject_GetAttrString(obj, "begin_mode");
    PyObject *args = execution_varargs_to_py(va);
    PyObject_CallObject(meth, args);

    Py_XDECREF(args);
    Py_XDECREF(meth);
}

#define RETURN_ON_KEY(a, b) do {     \
        emr.finished = a;            \
        emr.key_handled = b;         \
        goto ret;                    \
    } while(0)

static edit_mode_result_t edit_mode_python_on_key(edit_mode_t *em, key_event_t *key)
{
    edit_mode_result_t emr;

    DEBUG("inside edit_mode_python_on_key()\n");
    ASSERT(em->impl_type == EDIT_MODE_PYTHON_IMPL, "wrong impl");
    PyObject *obj = (PyObject *)em->impl;

    PyObject *meth = PyObject_GetAttrString(obj, "on_key");
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyInt_FromLong(key->key_code));
    PyObject *result = PyObject_CallObject(meth, args);

    if(!PyTuple_Check(result) || PyTuple_Size(result) != 2) {
        ERROR("Expected python on_key to return a tuple\n");
        RETURN_ON_KEY(true, false);
    }

    PyObject *bool1 = PyTuple_GET_ITEM(result, 0);
    PyObject *bool2 = PyTuple_GET_ITEM(result, 1);
    if(!PyBool_Check(bool1) || !PyBool_Check(bool2)){
        ERROR("Expected python on_key to return two booleans\n");
        RETURN_ON_KEY(true, false);
    }

    RETURN_ON_KEY(bool1 == Py_True, bool2 == Py_True);

    // cleanup
 ret:
    Py_XDECREF(result);
    Py_XDECREF(args);
    Py_XDECREF(meth);

    return emr;
}

static void edit_mode_python_destroy(edit_mode_t *em)
{
    ASSERT(em->impl_type == EDIT_MODE_PYTHON_IMPL, "wrong impl");
    // XXX - get this to free properly
    //PyObject *obj = (PyObject *)em->impl;
    //1Py_DECREF(obj);
    free(em);
}

static void register_new_mode_on_screen(pye_Screen *self, const char *mode_name, PyObject *mode_obj)
{
    edit_mode_t *em = calloc(1, sizeof(edit_mode_t));
    Py_INCREF(mode_obj);
    em->impl = mode_obj;
    em->impl_type = EDIT_MODE_PYTHON_IMPL;
    em->begin_mode = edit_mode_python_begin_mode;
    em->on_key = edit_mode_python_on_key;
    em->destroy = edit_mode_python_destroy;
    DEBUG("mode name: '%s'\n", mode_name);
    self->screen->add_mode(self->screen, mode_name, em);
}

static PyObject *Screen_add_mode(pye_Screen *self, PyObject *args)
{
    const char *mode_name;
    PyObject *mode_obj = NULL;
    if(!PyArg_ParseTuple(args, "sO", &mode_name, &mode_obj))
        return NULL;

    // verify the 'mode_obj' contains the right methods
    PyObject *begin_f = NULL, *on_key_f = NULL;

    begin_f = PyObject_GetAttrString(mode_obj, "begin_mode");
    if(!PyCallable_Check(begin_f)) {
        ERROR("Screen_add_mode recieved an object without callable 'begin_mode' parameter\n");
        goto ret;
    }

    on_key_f = PyObject_GetAttrString(mode_obj, "on_key");
    if(!PyCallable_Check(on_key_f)) {
        ERROR("Screen_add_mode recieved an object without callable 'on_key' parameter\n");
        goto ret;
    }


    register_new_mode_on_screen(self, mode_name, mode_obj);

 ret:
    Py_XDECREF(begin_f);
    Py_XDECREF(on_key_f);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Screen_trigger_mode(pye_Screen *self, PyObject *args)
{
    if(!PyTuple_Check(args) || PyTuple_Size(args) < 1) {
        ERROR("args should be a tuple of size >= 1 in Screen_trigger_mode()\n");
        return NULL;
    }

    PyObject *first = PyTuple_GET_ITEM(args, 0);
    ASSERT(PyString_Check(first), "Expected string in first arg of Screen_trigger_mode()");
    const char *mode_name = PyString_AsString(first);
    varargs_t *va = execution_varargs_to_c_skip(args, 1);
    self->screen->trigger_mode(self->screen, mode_name, va);


    varargs_destroy(va);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Screen_methods[] = {
    {"on_key", (PyCFunction)Screen_register_kbrd_callback, METH_VARARGS,
     "Register a key event handler for the screen object."},
    {"on_buffer_added", (PyCFunction)Screen_register_buf_callback, METH_VARARGS,
     "Register a buffer added event handler for the screen object."},
    {"list_buffers", (PyCFunction)Screen_list_buffers, METH_NOARGS,
     "Return a list of all buffer objects."},
    {"get_buffer", (PyCFunction)Screen_get_buffer, METH_VARARGS,
     "Get a buffer by its integer id number."},
    {"set_active_buffer", (PyCFunction)Screen_set_active_buffer, METH_VARARGS,
     "Set a the Screen's active Buffer by its integer id number."},
    {"get_active_buffer", (PyCFunction)Screen_get_active_buffer, METH_NOARGS,
     "Get the Screen's active Buffer."},
    {"refresh", (PyCFunction)Screen_refresh, METH_NOARGS,
     "Redraw the screen on the display."},
    {"get_viewport_line", (PyCFunction)Screen_get_viewport_line, METH_NOARGS,
     "Get the line number at the top of the screen (Note: this should really be a method of pye.Buffer)."},
    {"center_viewport", (PyCFunction)Screen_center_viewport, METH_NOARGS,
     "Center the buffer viewport on the current cursor position in the buffer (Note: this should really be a method of pye.Buffer)."},
    {"mb_write", (PyCFunction)Screen_mb_write, METH_VARARGS,
     "Write a message to the screen's Message buffer."},
    {"mb_get_yloc", (PyCFunction)Screen_mb_get_yloc, METH_NOARGS,
     "Get the y-coord of the message buffer."},
    {"add_mode", (PyCFunction)Screen_add_mode, METH_VARARGS,
     "Add a new edit mode to the screen object."},
    {"trigger_mode", (PyCFunction)Screen_trigger_mode, METH_VARARGS,
     "Trigger an edit mode registered with the screen object."},
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
