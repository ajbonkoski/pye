#ifndef SCREEN_H
#define SCREEN_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct screen screen_t;
struct screen
{
    void *impl;
    uint impl_type;

    void (*set_cursor)(screen_t *this, uint x, uint y);
    void (*get_cursor)(screen_t *this, uint *x, uint *y);
    void (*write)(screen_t *this, const char *s, size_t num);
    void (*destroy)(screen_t *this);
};

screen_t *screen_create_by_name(const char *name);

#ifdef __cplusplus
}
#endif

#endif
