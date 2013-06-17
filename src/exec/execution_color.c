#include "execution_color.h"
#include "execution_common.h"
#include "display/display.h"

/* #undef ENABLE_DEBUG */
/* #define ENABLE_DEBUG 1 */

typedef struct {
    PyObject_HEAD

    /* constants from "display/keyboard.h" */
    uint BLACK;
    uint RED;
    uint GREEN;
    uint YELLOW;
    uint BLUE;
    uint MAGENTA;
    uint CYAN;
    uint WHITE;

} pye_Color;

#define KEY_FIELD(key) \
    {#key, T_INT, offsetof(pye_Color, key), 0}

static PyMemberDef Color_members[] = {
    KEY_FIELD(BLACK),
    KEY_FIELD(RED),
    KEY_FIELD(GREEN),
    KEY_FIELD(YELLOW),
    KEY_FIELD(BLUE),
    KEY_FIELD(MAGENTA),
    KEY_FIELD(CYAN),
    KEY_FIELD(WHITE),
    {NULL}  /* Sentinel */
};

/* static PyObject *Keyboard_from_string(pye_Keyboard *self, PyObject *args) */
/* { */
/*     char c; */
/*     if(!PyArg_ParseTuple(args, "c", &c)) */
/*         return NULL; */

/*     int keycode = KBRD_CTRL(c); */
/*     return PyInt_FromLong(keycode); */
/* } */

static PyMethodDef Color_methods[] = {
    /* {"CTRL", (PyCFunction)Keyboard_CTRL, METH_VARARGS, */
    /*  "Get they keycode for some key when ctrl is held down."}, */
    {NULL}  /* Sentinel */
};

static PyTypeObject pye_ColorType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pye.Color",               /*tp_name*/
    sizeof(pye_Color),         /*tp_basicsize*/
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
    "Pye Color object",        /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Color_methods,             /* tp_methods */
    Color_members,             /* tp_members */
};


void execution_color_init(void)
{
    PYE_EXT_TYPE_INIT(pye_ColorType);
}


#define FIELD_ASSIGN(f) self->f = DISPLAY_COLOR_ ## f

PyObject *execution_color_create(void)
{
    PYE_EXT_TYPE_NEW(pye_ColorType, pye_Color);

    // do static init - this is probably the wrong place,
    // but Color should be a Singleton anyways
    FIELD_ASSIGN(BLACK);
    FIELD_ASSIGN(RED);
    FIELD_ASSIGN(GREEN);
    FIELD_ASSIGN(YELLOW);
    FIELD_ASSIGN(BLUE);
    FIELD_ASSIGN(MAGENTA);
    FIELD_ASSIGN(CYAN);
    FIELD_ASSIGN(WHITE);

    return (PyObject *)self;
}
