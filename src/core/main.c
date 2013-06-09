#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "screen/screen.h"
#include "screen/termio.h"

bool key_pressed(void *usr, key_event_t *e)
{
    screen_t *scrn = (screen_t *)usr;

    char c = e->key_code;
    char buffer[256];
    sprintf(buffer, "%c(%d) ", c, c);
    scrn->write(scrn, buffer, strlen(buffer));

    if(c == 'g')
        scrn->set_cursor(scrn, 0, 0);

    if(c == 's') {
        uint w, h;
        scrn->get_size(scrn, &w, &h);
        sprintf(buffer, "|w=%d h=%d|", w, h);
        scrn->write(scrn, buffer, strlen(buffer));
    }

    if(c == 'c') {
        scrn->clear(scrn);
    }

    return true;
}

void shutdown_func(void *usr)
{
    screen_t *this = (screen_t *)usr;
    this->destroy(this);
}

int main(int argc, char *argv[])
{
    screen_t *scrn = screen_create_by_name("terminal");
    set_crash_func(shutdown_func, scrn);
    scrn->register_kbrd_callback(scrn, key_pressed, scrn);
    scrn->main_loop(scrn);
    scrn->destroy(scrn);
    return 0;
}
