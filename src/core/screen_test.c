#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "display/display.h"
#include "screen/screen.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define KEY_CTRL(ch) (ch-'a'+1)

typedef struct
{
    display_t *disp;
    screen_t *scrn;

} app_t;

bool key_pressed(void *usr, key_event_t *e)
{
    app_t *this = (app_t *)usr;

    u32 c = e->key_code;
    char ch = (char)c;
    char buffer[256];

    uint w, h, x, y;
    this->disp->get_size(this->disp, &w, &h);
    this->disp->get_cursor(this->disp, &x, &y);

    if(c == KEY_CTRL('s')) {
        sprintf(buffer, "|w=%d h=%d|", w, h);
        this->scrn->write_mb(this->scrn, buffer);
    }

    else if(c == KEY_CTRL('l')) {
        sprintf(buffer, "|x=%d y=%d|", x, y);
        this->scrn->write_mb(this->scrn, buffer);
    }

    else if(c == KEY_CTRL('g')) {
        this->scrn->write_mb(this->scrn, NULL);
    }

    else if(c == KBRD_ARROW_LEFT) {
        if(x > 0)
            this->disp->set_cursor(this->disp, x-1, y);
    }

    else if(c == KBRD_ARROW_RIGHT) {
        if(x < w-1)
            this->disp->set_cursor(this->disp, x+1, y);

    }

    else if(c == KBRD_ARROW_UP) {
        if(y > 0)
            this->disp->set_cursor(this->disp, x, y-1);

    }

    else if(c == KBRD_ARROW_DOWN) {
        if(y < w-2)
            this->disp->set_cursor(this->disp, x, y+1);

    }

    else {
        this->disp->write(this->disp, &ch, 1);
    }

    DEBUG("%d:%d:%d:%d\n", c, KEY_CTRL('s'), KEY_CTRL('l'), KEY_CTRL('g'));

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
