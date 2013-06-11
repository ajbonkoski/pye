#ifndef DISPLAY_CURSES_H
#define DISPLAY_CURSES_H

#include "common/common.h"
#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

display_t *display_curses_create(void);

#ifdef __cplusplus
}
#endif

#endif
