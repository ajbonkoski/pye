#include "common/common.h"
#include "display/display.h"
#include "screen/screen.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define DISPLAY_TYPE "curses"

typedef struct
{
    display_t *disp;
    screen_t *scrn;
    buffer_t *buf;

} app_t;

int main(int argc, char *argv[])
{
    app_t this;
    this.disp = display_create_by_name(DISPLAY_TYPE);
    this.scrn = screen_create(this.disp);
    this.buf = buffer_create();
    this.scrn->add_buffer(this.scrn, this.buf);

    this.disp->main_loop(this.disp);

    this.buf->destroy(this.buf);
    this.scrn->destroy(this.scrn);
    this.disp->destroy(this.disp);
    return 0;
}
