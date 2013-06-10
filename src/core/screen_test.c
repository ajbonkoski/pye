#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "display/display.h"
#include "screen/screen.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct
{
    display_t *disp;
    screen_t *scrn;

} app_t;

bool key_pressed(void *usr, key_event_t *e)
{
    app_t *this = (app_t *)usr;

    char c = e->key_code;
    char buffer[256];

    if(c == 'g')
        this->disp->set_cursor(this->disp, 0, 0);

    else if(c == 's') {
        uint w, h;
        this->disp->get_size(this->disp, &w, &h);
        sprintf(buffer, "|w=%d h=%d|", w, h);
        this->scrn->write_mb(this->scrn, buffer);
    }

    else if(c == 'l') {
        uint x, y;
        this->disp->get_cursor(this->disp, &x, &y);
        sprintf(buffer, "|x=%d y=%d|", x, y);
        this->scrn->write_mb(this->scrn, buffer);
    }

    else if(c == 'c') {
        this->scrn->write_mb(this->scrn, NULL);
    }

    else {
        this->disp->write(this->disp, &c, 1);
    }

    return true;
}

int main(int argc, char *argv[])
{
    app_t this;
    this.disp = display_create_by_name("terminal");
    this.disp->register_kbrd_callback(this.disp, key_pressed, &this);
    this.scrn = screen_create(this.disp);

    this.disp->main_loop(this.disp);

    this.scrn->destroy(this.scrn);
    this.disp->destroy(this.disp);
    return 0;
}
