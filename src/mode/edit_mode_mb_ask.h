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
      mb_response_func_t func       The callback to notify on completion
      void *usr                     The callback's data (if needed)
****************************************/

#ifdef __cplusplus
}
#endif

#endif
