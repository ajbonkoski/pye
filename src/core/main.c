#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "screen/screen.h"
#include "screen/termio.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

bool key_pressed(void *usr, key_event_t *e)
{
    screen_t *scrn = (screen_t *)usr;

    char c = e->key_code;
    //char buffer[256];

    if(c == 'g')
        scrn->set_cursor(scrn, 0, 0);

    else if(c == 's') {
        uint w, h;
        scrn->get_size(scrn, &w, &h);
        DEBUG("|w=%d h=%d|\n", w, h);
        //scrn->write(scrn, buffer, strlen(buffer));
    }

    else if(c == 'c') {
        scrn->clear(scrn);
    }

    else if(c == 'l') {
        uint x, y;
        scrn->get_cursor(scrn, &x, &y);
        DEBUG("|x=%d y=%d|", x, y);
        //scrn->write(scrn, buffer, strlen(buffer));
    }

    else {
        scrn->write(scrn, &c, 1);
    }

    return true;
}

int main(int argc, char *argv[])
{
    screen_t *scrn = screen_create_by_name("terminal");
    scrn->register_kbrd_callback(scrn, key_pressed, scrn);
    scrn->main_loop(scrn);
    scrn->destroy(scrn);
    return 0;
}
