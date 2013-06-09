#include "screen.h"

#include <string.h>
#include <signal.h>

#include "screen_terminal.h"

#define KEY_CTRL_C 3
#define KEY_CTRL_Z 26
#define KEY_CTRL_SLASH 28

// the standard event handler, to implement basic Unix interupt key commands
static bool key_handler(void *usr, key_event_t *e)
{
    screen_t *this = (screen_t *)usr;

    switch(e->key_code) {
        case KEY_CTRL_C:
            this->main_quit(this);
            return true;
        case KEY_CTRL_Z:
            raise(SIGSTOP);
            return true;
    }

    return false;
}

screen_t *screen_create_by_name(const char *name)
{
    #define CHECK(str, f) if(strcmp(str, name) == 0) s = f();
    screen_t *s = NULL;

    CHECK("terminal", screen_terminal_create);

    if(s != NULL)
        s->register_kbrd_callback(s, key_handler, s);

    else {
        ERROR("Failed to recognize '%s' in screen_create_by_name\n", name);
        ASSERT_FAIL("Fatal Error");
    }

    return s;
    #undef CHECK
}
