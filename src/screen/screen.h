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

    void (*destroy)(screen_t *this);
};

screen_t *screen_create(void);

#ifdef __cplusplus
}
#endif

#endif
