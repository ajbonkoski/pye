#include "kbrd_curses.h"

#include <curses.h>

struct kbrd_curses {};

kbrd_curses_t *kbrd_curses_create(void)
{
    return NULL;
}

void kbrd_curses_destroy(kbrd_curses_t *this)
{

}

static key_event_t *make_event(key_event_t *ke, u32 mod, u32 kc, u32 rel)
{
    if(ke == NULL)
        ke = malloc(sizeof(key_event_t));
    ke->modifiers = mod;
    ke->key_code = kc;
    ke->released = rel;
    return ke;
}

key_event_t *kbrd_curses_keycode(kbrd_curses_t *this, int ch, key_event_t *e)
{
    // normal keys
    if(ch <= 127)
        return make_event(e, 0, ch, 0);

    // special keys
    switch(ch) {
        case KEY_DOWN:       return make_event(e, 0, KBRD_ARROW_DOWN, 0);
        case KEY_UP:         return make_event(e, 0, KBRD_ARROW_UP, 0);
        case KEY_LEFT:       return make_event(e, 0, KBRD_ARROW_LEFT, 0);
        case KEY_RIGHT:      return make_event(e, 0, KBRD_ARROW_RIGHT, 0);
        case KEY_HOME:       return make_event(e, 0, KBRD_HOME, 0);
        case KEY_BACKSPACE:  return make_event(e, 0, KBRD_BACKSPACE, 0);
        case KEY_F(1):       return make_event(e, 0, KBRD_F1, 0);
        case KEY_F(2):       return make_event(e, 0, KBRD_F2, 0);
        case KEY_F(3):       return make_event(e, 0, KBRD_F3, 0);
        case KEY_F(4):       return make_event(e, 0, KBRD_F4, 0);
        case KEY_F(5):       return make_event(e, 0, KBRD_F5, 0);
        case KEY_F(6):       return make_event(e, 0, KBRD_F6, 0);
        case KEY_F(7):       return make_event(e, 0, KBRD_F7, 0);
        case KEY_F(8):       return make_event(e, 0, KBRD_F8, 0);
        case KEY_F(9):       return make_event(e, 0, KBRD_F9, 0);
        case KEY_F(10):      return make_event(e, 0, KBRD_F10, 0);
        case KEY_F(11):      return make_event(e, 0, KBRD_F11, 0);
        case KEY_F(12):      return make_event(e, 0, KBRD_F12, 0);
        case KEY_DC:         return make_event(e, 0, KBRD_DEL, 0);
        case KEY_IC:         return make_event(e, 0, KBRD_INS, 0);
        case KEY_ENTER:      return make_event(e, 0, KBRD_ENTER, 0);
        default:
            ERROR("Unrecognized key %x\n", ch);
            return NULL;
    }
}

