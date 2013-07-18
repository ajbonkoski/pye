#include "Python.h"
#include "execution.h"
#include "execution_display.h"
#include "execution_keyboard.h"
#include "execution_color.h"
#include "execution_screen.h"
#include "execution_buffer.h"
#include "execution_kill_buffer.h"

#define CONFIG_MODULE "config"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

struct execution
{
    const char *pye_script_dir;
    PyObject *config_module;
};

static void execution_init(void)
{
    execution_display_init();
    execution_keyboard_init();
    execution_color_init();
    execution_screen_init();
    execution_buffer_init();
    execution_kill_buffer_init();
}

static PyObject *pye_debug(PyObject *self, PyObject *args)
{
    const char *str;

    if(!PyArg_ParseTuple(args, "s", &str))
        return NULL;

    DEBUG("%s\n", str);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *pye_error(PyObject *self, PyObject *args)
{
    const char *str;

    if(!PyArg_ParseTuple(args, "s", &str))
        return NULL;

    ERROR("%s\n", str);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyeMethods[] = {
    {"debug", pye_debug, METH_VARARGS,
    "Print debug message using the pye debug infustructure."},
    {"error", pye_error, METH_VARARGS,
    "Print error message using the pye error infustructure."},
    {NULL, NULL, 0, NULL}
};

static void sys_path_append(const char *pye_script_dir)
{
    PyObject *path = PySys_GetObject("path");
    PyList_Append(path, PyString_FromString(pye_script_dir));
}

static PyObject *load_module(const char *pye_script_dir, const char *s)
{
    sys_path_append(pye_script_dir);
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

execution_t *execution_create(const char *pye_script_dir, screen_t *scrn, display_t *disp)
{
    kill_buffer_t *kb = scrn->get_kill_buffer(scrn);

    execution_t *this = calloc(1, sizeof(execution_t));
    this->pye_script_dir = pye_script_dir;

    Py_Initialize();
    execution_init();

    PyObject *m = Py_InitModule("pye", PyeMethods);
    PyModule_AddObject(m, "display", execution_display_create(disp));
    PyModule_AddObject(m, "keyboard", execution_keyboard_create());
    PyModule_AddObject(m, "color", execution_color_create());
    PyModule_AddObject(m, "screen",  execution_screen_create(scrn));
    PyModule_AddObject(m, "killbuffer",  execution_kill_buffer_create(kb));

    this->config_module = load_module(pye_script_dir, CONFIG_MODULE);

    return this;
}

void execution_destroy(execution_t *this)
{
    if(this->config_module)
        Py_DECREF(this->config_module);
    Py_Finalize();

    free(this);
}
