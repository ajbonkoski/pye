#ifndef KBRD_TERMINAL_H
#define KBRD_TERMINAL_H

#include "common/common.h"
#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kbrd_terminal kbrd_terminal_t;

kbrd_terminal_t *kbrd_terminal_create(void);
void kbrd_terminal_destroy(kbrd_terminal_t *this);

// If 'e' is not null, it is used for the return value. Otherwise, memory is malloc'd. NULL is returned if no key code is generated.
key_event_t *kbrd_terminal_keycode(kbrd_terminal_t *this, char ch, key_event_t *e);


#ifdef __cplusplus
}
#endif

#endif
