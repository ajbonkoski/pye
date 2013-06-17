#include "Python.h"
#include "execution.h"
#include "execution_display.h"
#include "execution_keyboard.h"
#include "execution_color.h"
#include "execution_screen.h"
#include "execution_buffer.h"

#define CONFIG_MODULE "config"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

struct execution
{
    PyObject *config_module;
};

static PyObject *pye_debug(PyObject *self, PyObject *args)
{
    const char *str;

    if(!PyArg_ParseTuple(args, "s", &str))
        return NULL;

    DEBUG("%s\n", str);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyeMethods[] = {
    {"debug", pye_debug, METH_VARARGS,
    "Print debug message using the pye debug infustructure."},
    {NULL, NULL, 0, NULL}
};

static PyObject *load_module(const char *s)
{
    PyObject *name = PyString_FromString(s);
    PyObject *module = PyImport_Import(name);
    Py_DECREF(name);
    if(module == NULL) {
        ERROR("Failed to load config module: %s\n", s);
        if(PyErr_Occurred() != NULL) {
            PyErr_Print();
        }
    }
    return module;
}

execution_t *execution_create(screen_t *scrn, display_t *disp)
{
    execution_t *this = calloc(1, sizeof(execution_t));

    Py_Initialize();
    execution_display_init();
    execution_keyboard_init();
    execution_color_init();
    execution_screen_init();
    execution_buffer_init();

    PyObject *m = Py_InitModule("pye", PyeMethods);
    PyModule_AddObject(m, "display", execution_display_create(disp));
    PyModule_AddObject(m, "keyboard", execution_keyboard_create());
    PyModule_AddObject(m, "color", execution_color_create());
    PyModule_AddObject(m, "screen",  execution_screen_create(scrn));

    this->config_module = load_module(CONFIG_MODULE);

    return this;
}

void execution_destroy(execution_t *this)
{
    if(this->config_module)
        Py_DECREF(this->config_module);
    Py_Finalize();

    free(this);
}
