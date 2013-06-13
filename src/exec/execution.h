#ifndef EXECUTION_H
#define EXECUTION_H

#include "common/common.h"
#include "display/display.h"
#include "screen/screen.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct execution execution_t;
execution_t *execution_create(screen_t *scrn, display_t *disp);
void execution_destroy(execution_t *this);

#ifdef __cplusplus
}
#endif

#endif
