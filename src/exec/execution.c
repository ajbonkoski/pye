#include "Python.h"
#include "execution.h"

#define CONFIG_MODULE "config"

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

struct execution
{
    PyObject *config_module;
};

static PyObject *load_module(const char *s)
{
    PyObject *name = PyString_FromString(s);
    PyObject *module = PyImport_Import(name);
    Py_DECREF(name);
    if(module == NULL) {
        DEBUG("Failed to load config module: %s\n", s);
    }
    return module;
}

execution_t *execution_create(screen_t *scrn, display_t *disp)
{
    execution_t *this = calloc(1, sizeof(execution_t));

    Py_Initialize();
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
