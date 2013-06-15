#include "execution_buffer.h"
#include "buffer/data_buffer.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD
    buffer_t *buffer;
    data_buffer_t *dbuffer;
    PyObject *callback_func; // XXX: this is not cleaned up properly
} pye_Buffer;


static PyObject *Buffer_get_cursor(pye_Buffer *self)
{
    uint x, y;
    self->dbuffer->get_cursor(self->dbuffer, &x, &y);

    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, PyInt_FromLong((long)x));
    PyTuple_SetItem(tuple, 1, PyInt_FromLong((long)y));
    return tuple;
}

static PyObject *Buffer_set_cursor(pye_Buffer *self, PyObject *args)
{
    uint x, y;
    if(!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;

    self->dbuffer->set_cursor(self->dbuffer, x, y);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Buffer_insert(pye_Buffer *self, PyObject *args)
{
    int c;
    if(!PyArg_ParseTuple(args, "i", &c))
        return NULL;

    self->dbuffer->insert(self->dbuffer, c);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Buffer_get_line_data(pye_Buffer *self, PyObject *args)
{
    const uint BUFSZ = 200;
    char buffer[BUFSZ];
    char *alloc = NULL;

    uint i;
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

    uint ll = self->dbuffer->line_len(self->dbuffer, i);

    // setup the data pointer
    char *ptr = buffer;
    if(ll+1 > BUFSZ) {
        alloc = malloc((ll+1) * sizeof(char));
        ptr = alloc;
    }

    data_buffer_t *db = self->dbuffer;
    char *str = db->get_line_data(db, i, ptr);

    return PyString_FromString(str);
}

static buffer_line_t *buffer_formatter_handler(void *usr, char *data)
{
    PyObject *func = (PyObject *)usr;
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyString_FromString(data));
    PyObject *result = PyObject_CallObject(func, args);

    PyObject *pData = NULL;
    PyObject *pStyles = NULL;
    PyObject *pRegions = NULL;

    buffer_line_t *bl = calloc(1, sizeof(buffer_line_t));
    bl->styles = NULL;
    bl->regions = NULL;
    bl->data = data;

    if(!PyDict_Check(result)) {
        ERROR("python key handler did not return a dictionary\n");
        goto done;
    }

    // these are "borrowed" references
    pData    = PyDict_GetItemString(result, "data");
    pStyles  = PyDict_GetItemString(result, "styles");
    pRegions = PyDict_GetItemString(result, "regions");

    // to shut up the compiler
    if(0) {pStyles = pRegions; pRegions = pStyles;}

    if(!pData || !PyString_Check(pData)) {
        ERROR("python key handler: returned dictionary has no 'data' key of type string\n");
        goto done;
    }

    // extract and set the returned data
    bl->data = strdup(PyString_AsString(pData));

    // check for styles
    if(pStyles == NULL) {
        DEBUG("Note: Formatter styles not set in buffer_formatter_handler()\n");
        goto done;
    }

    // make sure the styles have the right type
    if(!PyDict_Check(pStyles)) {
        ERROR("python fmt handler: expected pStyles to be a dictionary type!\n");
        goto done;
    }


    // some cleanup
 done:
    Py_DECREF(result);
    Py_DECREF(args);

    return bl;
}

static PyObject *Buffer_register_formatter(pye_Buffer *self, PyObject *args)
{
    PyObject *func = NULL;
    if(!PyArg_ParseTuple(args, "O", &func))
        return NULL;

    if(!PyCallable_Check(func)) {
        ERROR("error: Buffer_register_formatter recieved uncallable parameter\n");
        Py_XDECREF(func);
        goto ret;
    }

    DEBUG("Buffer_register_formatter: registering...\n");

    buffer_t *b = self->buffer;
    b->register_formatter(b, buffer_formatter_handler, (void *)func);
    self->callback_func = func;

 ret:
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Buffer_line_len(pye_Buffer *self, PyObject *args)
{
    uint i;
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

    uint ll = self->dbuffer->line_len(self->dbuffer, i);
    return PyInt_FromLong(ll);
}

static PyObject *Buffer_get_char_at(pye_Buffer *self, PyObject *args)
{
    /* char (*get_char_at)(data_buffer_t *this, uint x, uint y); */
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Buffer_num_lines(pye_Buffer *self)
{
    uint nlines = self->dbuffer->num_lines(self->dbuffer);
    return PyInt_FromLong(nlines);
}

static PyMethodDef Buffer_methods[] = {
    {"get_cursor", (PyCFunction)Buffer_get_cursor, METH_NOARGS,
     "Get the cursor location in the buffer. This can differ from the screen cursor."},
    {"set_cursor", (PyCFunction)Buffer_set_cursor, METH_VARARGS,
     "Set the cursor location in the buffer. This can differ from the screen cursor."},
    {"insert", (PyCFunction)Buffer_insert, METH_VARARGS,
     "Insert a character into the buffer at the current cursor location. Note: does not automatically redraw screen."},
    {"get_line_data", (PyCFunction)Buffer_get_line_data, METH_VARARGS,
     "Get a string of the data on the requested line."},
    {"register_formatter", (PyCFunction)Buffer_register_formatter, METH_VARARGS,
     " Register a function to be called when the application asks for a formatted line. The registered function gets a string with the contents of the line, and should return a new formatted string."},
    {"line_len", (PyCFunction)Buffer_line_len, METH_VARARGS,
     "Get a length of the data on the requested line."},
    {"get_char_at", (PyCFunction)Buffer_get_char_at, METH_VARARGS,
     "Get the character at the requested x, y location."},
    {"num_lines", (PyCFunction)Buffer_num_lines, METH_NOARGS,
     "Get the number of lines contained in the buffer."},
    {NULL}  /* Sentinel */
};

static PyTypeObject pye_BufferType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pye.Buffer",              /*tp_name*/
    sizeof(pye_Buffer),        /*tp_basicsize*/
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
    "Pye Buffer object",       /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Buffer_methods,            /* tp_methods */
};


void execution_buffer_init(void)
{
    pye_BufferType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pye_BufferType) < 0)
        return;

    Py_INCREF(&pye_BufferType);
}

PyObject *execution_buffer_create(buffer_t *buf)
{
    PyTypeObject *t = &pye_BufferType;
    pye_Buffer *self = (pye_Buffer *)t->tp_alloc(t, 0);
    if(self == NULL) {
        DEBUG("Failed to make a pye_Buffer object\n");
        return NULL;
    }

    self->buffer = buf;
    self->dbuffer = buf->get_data_buffer(buf);
    return (PyObject *)self;
}
