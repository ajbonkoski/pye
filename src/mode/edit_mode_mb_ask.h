#ifndef EDIT_MODE_MB_ASK_H
#define EDIT_MODE_MB_ASK_H

#include "common/common.h"
#include "edit_mode.h"

#include "screen/screen.h"

#ifdef __cplusplus
extern "C" {
#endif

edit_mode_t *edit_mode_mb_ask_create(screen_t *screen);

/******* begin_mode() args: ************
      const char *str               The "question" to ask
      const callable_t *callable    The callback****************************************/

#ifdef __cplusplus
}
#endif

#endif
