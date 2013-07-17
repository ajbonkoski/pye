#include "varargs.h"
#include "common/varray.h"
#include "common/callable.h"

#define MAX_SIZE 128

struct varargs
{
    char fmt[MAX_SIZE];
    uint size;
    varray_t *args;
};

varargs_t *varargs_create(void)
{
    varargs_t *va = calloc(1, sizeof(varargs_t));
    va->size = 0;
    va->args = varray_create();
    return va;
}

varargs_t *varargs_create_v(uint num, ...)
{
    varargs_t *va = varargs_create();

    va_list args;
    va_start(args, num);

    for(uint i = 0; i < num; i++) {
        const char *c = va_arg(args, const char *);
        void *d = va_arg(args, void *);
        varargs_add(va, *c, d);
    }

    va_end(args);

    return va;
}

void varargs_destroy(varargs_t *this)
{
    for(uint i = 0; i < this->size; i++) {
        if(this->fmt[i] == 'c')
            callable_destroy(varray_get(this->args, i));
    }

    varray_destroy(this->args);
    free(this);
}

void varargs_add(varargs_t *this, char type_ch, void *data)
{
    ASSERT(this->size < MAX_SIZE-1, "varargs_add(): exceeded maximum range");
    uint i = this->size++;
    this->fmt[i] = type_ch;
    varray_add(this->args, data);
}

void *varargs_get(varargs_t *this, uint i)
{
    ASSERT(i < this->size, "varargs_get(): out of range");
    return varray_get(this->args, i);
}

char varargs_get_type(varargs_t *this, uint i)
{
    ASSERT(i < this->size, "varargs_get_type(): out of range");
    return this->fmt[i];
}

uint varargs_size(varargs_t *this)
{
    return this->size;
}
