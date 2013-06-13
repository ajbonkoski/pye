#include "execution_buffer.h"
#include "buffer/data_buffer.h"
//#include "display/display.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD
    buffer_t *buffer;
    data_buffer_t *dbuffer;
} pye_Buffer;


static PyObject *Buffer_insert(pye_Buffer *self, PyObject *args)
{
    char c;
    if(!PyArg_ParseTuple(args, "c", &c))
        return NULL;

    self->dbuffer->insert(self->dbuffer, (int)c);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Buffer_methods[] = {
    {"insert", (PyCFunction)Buffer_insert, METH_VARARGS,
     "Insert a character into the buffer at the current cursor location. Note: does not automatically redraw screen."},
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
