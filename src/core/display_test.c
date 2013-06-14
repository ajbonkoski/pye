#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "display/display.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define DISPLAY_TYPE "curses"

bool key_pressed(void *usr, key_event_t *e)
{
    display_t *disp = (display_t *)usr;

    char c = e->key_code;
    //char buffer[256];

    if(c == 'g')
        disp->set_cursor(disp, 0, 0);

    else if(c == 's') {
        uint w, h;
        disp->get_size(disp, &w, &h);
        DEBUG("|w=%d h=%d|\n", w, h);
        //disp->write(disp, buffer, strlen(buffer), -1);
    }

    else if(c == 'c') {
        disp->clear(disp);
    }

    else if(c == 'l') {
        uint x, y;
        disp->get_cursor(disp, &x, &y);
        DEBUG("|x=%d y=%d|", x, y);
        //disp->write(disp, buffer, strlen(buffer), -1);
    }

    else {
        disp->write(disp, &c, 1, -1);
    }

    return true;
}

int main(int argc, char *argv[])
{
    display_t *disp = display_create_by_name(DISPLAY_TYPE);
    disp->register_kbrd_callback(disp, key_pressed, disp);
    disp->main_loop(disp);
    disp->destroy(disp);
    return 0;
}
