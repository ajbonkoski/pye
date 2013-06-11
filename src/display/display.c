#include "display.h"

#include <string.h>
#include <signal.h>

#include "display_terminal.h"
#include "display_curses.h"

#define KEY_CTRL_C 3
#define KEY_CTRL_Z 26
#define KEY_CTRL_SLASH 28

// the standard event handler, to implement basic Unix interupt key commands
/* static bool key_handler(void *usr, key_event_t *e) */
/* { */
/*     display_t *this = (display_t *)usr; */

/*     switch(e->key_code) { */
/*         case KEY_CTRL_C: */
/*             this->main_quit(this); */
/*             return true; */
/*         case KEY_CTRL_Z: */
/*             raise(SIGSTOP); */
/*             return true; */
/*     } */

/*     return false; */
/* } */

display_t *display_create_by_name(const char *name)
{
    #define CHECK(str, f) if(strcmp(str, name) == 0) d = f();
    display_t *d = NULL;

    CHECK("terminal", display_terminal_create);
    CHECK("curses",   display_curses_create);

    /* if(d != NULL) */
    /*     d->register_kbrd_callback(d, key_handler, d); */

    /* else { */
    if(d == NULL) {
        ERROR("Failed to recognize '%s' in display_create_by_name\n", name);
        ASSERT_FAIL("Fatal Error");
    }
    /* } */

    return d;
    #undef CHECK
}
