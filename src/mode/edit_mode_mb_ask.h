#ifndef EDIT_MODE_MB_ASK_H
#define EDIT_MODE_MB_ASK_H

#include "common/common.h"
#include "edit_mode.h"

#include "screen/screen.h"

#ifdef __cplusplus
extern "C" {
#endif

// returns heap-mem, caller is responsible for cleanup
typedef char *(*autocomplete_f)(const char *input, void *usr);

edit_mode_t *edit_mode_mb_ask_create(screen_t *screen);

/******* begin_mode() varargs: ************
      s: const char *str               The "question" to ask
      c: const callable_t *callable    The callback

      // optional args
      o: char *autocomplete_f(const char *s, void *usr)
      o: void *autocomplete_usr
****************************************/

#ifdef __cplusplus
}
#endif

#endif
