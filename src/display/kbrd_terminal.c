#include "kbrd_terminal.h"

#define MAGIC 91
#define SPECIAL_MODE_KEY 27
#define SPECIAL_ARROW_LEFT  65
#define SPECIAL_ARROW_RIGHT 66
#define SPECIAL_ARROW_UP    67
#define SPECIAL_ARROW_DOWN  68

struct kbrd_terminal
{
    bool special_mode;
    char special_ch;
};

kbrd_terminal_t *kbrd_terminal_create(void)
{
    kbrd_terminal_t *this = calloc(1, sizeof(kbrd_terminal_t));
    this->special_mode = false;
    this->special_ch = '\0';

    return this;
}

void kbrd_terminal_destroy(kbrd_terminal_t *this)
{
    free(this);
}

static key_event_t *make_event(key_event_t *e, u32 mod, u32 kc, u32 rel)
{
    if(e == NULL)
        e = calloc(1, sizeof(key_event_t));
    e->modifiers = mod;
    e->key_code = kc;
    e->released = rel;
    return e;
}

static key_event_t *make_special_event(kbrd_terminal_t *this, char ch, key_event_t *e)
{
    key_event_t *ret = NULL;

    if(this->special_ch != MAGIC) {
        ERROR("Unrecognized special_ch: %c\n", this->special_ch);
        ret = NULL;
        goto done;
    }

    switch(ch) {
        case SPECIAL_ARROW_LEFT:
            ret = make_event(e, 0, KBRD_ARROW_LEFT, 0);
            break;
        case SPECIAL_ARROW_RIGHT:
            ret = make_event(e, 0, KBRD_ARROW_RIGHT, 0);
            break;
        case SPECIAL_ARROW_UP:
            ret = make_event(e, 0, KBRD_ARROW_UP, 0);
            break;
        case SPECIAL_ARROW_DOWN:
            ret = make_event(e, 0, KBRD_ARROW_DOWN, 0);
            break;
        default:
            ERROR("Unrecognized ch: %c\n", ch);
            ret = NULL;
    }

 done:
    this->special_ch = '\0';
    this->special_mode = false;
    return ret;
}

static key_event_t *make_normal_event(kbrd_terminal_t *this, char ch, key_event_t *e)
{
    return make_event(e, 0, ch, 0);
}

key_event_t *kbrd_terminal_keycode(kbrd_terminal_t *this, char ch, key_event_t *e)
{
    if(this->special_mode) {
        if(this->special_ch == '\0') {
            this->special_ch = ch;
            return NULL;
        } else {
            return make_special_event(this, ch, e);
        }
    } else {
        if(ch == SPECIAL_MODE_KEY) {
            this->special_mode = true;
            return NULL;
        } else {
            return make_normal_event(this, ch, e);
        }
    }
}

