#ifndef KBRD_CURSES_H
#define KBRD_CURSES_H

#include "common/common.h"
#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kbrd_curses kbrd_curses_t;

kbrd_curses_t *kbrd_curses_create(void);
void kbrd_curses_destroy(kbrd_curses_t *this);

// If 'e' is not null, it is used for the return value. Otherwise, memory is malloc'd. NULL is returned if no key code is generated.
key_event_t *kbrd_curses_keycode(kbrd_curses_t *this, int ch, key_event_t *e);


#ifdef __cplusplus
}
#endif

#endif
