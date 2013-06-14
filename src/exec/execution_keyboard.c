#include "execution_keyboard.h"
#include "display/keyboard.h"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct {
    PyObject_HEAD

    /* constants from "display/keyboard.h" */
    uint ARROW_LEFT;
    uint ARROW_RIGHT;
    uint ARROW_UP;
    uint ARROW_DOWN;
    uint HOME;
    uint BACKSPACE;
    uint F1;
    uint F2;
    uint F3;
    uint F4;
    uint F5;
    uint F6;
    uint F7;
    uint F8;
    uint F9;
    uint F10;
    uint F11;
    uint F12;
    uint DEL;
    uint INS;
    uint ENTER;

} pye_Keyboard;

#define KEY_FIELD(key) \
    {#key, T_INT, offsetof(pye_Keyboard, key), 0}

static PyMemberDef Keyboard_members[] = {
    KEY_FIELD(ARROW_LEFT),
    KEY_FIELD(ARROW_RIGHT),
    KEY_FIELD(ARROW_UP),
    KEY_FIELD(ARROW_DOWN),
    KEY_FIELD(HOME),
    KEY_FIELD(BACKSPACE),
    KEY_FIELD(F1),
    KEY_FIELD(F2),
    KEY_FIELD(F3),
    KEY_FIELD(F4),
    KEY_FIELD(F5),
    KEY_FIELD(F6),
    KEY_FIELD(F7),
    KEY_FIELD(F8),
    KEY_FIELD(F9),
    KEY_FIELD(F10),
    KEY_FIELD(F11),
    KEY_FIELD(F12),
    KEY_FIELD(DEL),
    KEY_FIELD(INS),
    KEY_FIELD(ENTER),
    {NULL}  /* Sentinel */
};

static PyObject *Keyboard_CTRL(pye_Keyboard *self, PyObject *args)
{
    char c;
    if(!PyArg_ParseTuple(args, "c", &c))
        return NULL;

    int keycode = KBRD_CTRL(c);
    return PyInt_FromLong(keycode);
}

static PyMethodDef Keyboard_methods[] = {
    {"CTRL", (PyCFunction)Keyboard_CTRL, METH_VARARGS,
     "Get they keycode for some key when ctrl is held down."},
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
    Keyboard_members,          /* tp_members */
};


void execution_keyboard_init(void)
{
    pye_KeyboardType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pye_KeyboardType) < 0)
        return;

    Py_INCREF(&pye_KeyboardType);
}


#define KEY_ASSIGN(key) self->key = KBRD_ ## key

PyObject *execution_keyboard_create(void)
{
    PyTypeObject *t = &pye_KeyboardType;
    pye_Keyboard *self = (pye_Keyboard *)t->tp_alloc(t, 0);
    if(self == NULL) {
        DEBUG("Failed to make a pye_Keyboard object\n");
        return NULL;
    }

    // do static init - this is probably the wrong place,
    // but Keyboard should be a Singleton anyways
    KEY_ASSIGN(ARROW_LEFT);
    KEY_ASSIGN(ARROW_RIGHT);
    KEY_ASSIGN(ARROW_UP);
    KEY_ASSIGN(ARROW_DOWN);
    KEY_ASSIGN(HOME);
    KEY_ASSIGN(BACKSPACE);
    KEY_ASSIGN(F1);
    KEY_ASSIGN(F2);
    KEY_ASSIGN(F3);
    KEY_ASSIGN(F4);
    KEY_ASSIGN(F5);
    KEY_ASSIGN(F6);
    KEY_ASSIGN(F7);
    KEY_ASSIGN(F8);
    KEY_ASSIGN(F9);
    KEY_ASSIGN(F10);
    KEY_ASSIGN(F11);
    KEY_ASSIGN(F12);
    KEY_ASSIGN(DEL);
    KEY_ASSIGN(INS);
    KEY_ASSIGN(ENTER);

    return (PyObject *)self;
}
