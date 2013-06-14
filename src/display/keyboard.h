#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u32 modifiers;
    u32 key_code;
    u32 released;

} key_event_t;

#define KBRD_CTRL(ch)   (ch-'a'+1)
#define KBRD_ARROW_LEFT     ((1)<<8)
#define KBRD_ARROW_RIGHT    ((2)<<8)
#define KBRD_ARROW_UP       ((3)<<8)
#define KBRD_ARROW_DOWN     ((4)<<8)
#define KBRD_HOME           ((5)<<8)
#define KBRD_BACKSPACE      ((6)<<8)
#define KBRD_F1             ((7)<<8)
#define KBRD_F2             ((8)<<8)
#define KBRD_F3             ((9)<<8)
#define KBRD_F4            ((10)<<8)
#define KBRD_F5            ((11)<<8)
#define KBRD_F6            ((12)<<8)
#define KBRD_F7            ((13)<<8)
#define KBRD_F8            ((14)<<8)
#define KBRD_F9            ((15)<<8)
#define KBRD_F10           ((16)<<8)
#define KBRD_F11           ((17)<<8)
#define KBRD_F12           ((18)<<8)
#define KBRD_DEL           ((19)<<8)
#define KBRD_INS           ((20)<<8)
#define KBRD_ENTER         ((21)<<8)

// true means "handled", false means "not handled"
typedef bool (*key_event_func_t)(void *usr, key_event_t *e);

#ifdef __cplusplus
}
#endif

#endif
