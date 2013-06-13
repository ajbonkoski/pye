#include "common/common.h"
#include "display/display.h"
#include "screen/screen.h"
#include "fileio/fileio.h"
#include "exec/execution.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define DISPLAY_TYPE "curses"

typedef struct
{
    display_t   *disp;
    screen_t    *scrn;
    execution_t *exec;
    buffer_t    *buf;

} app_t;

int main(int argc, char *argv[])
{
    // parse args
    const char *filename = NULL;
    if(argc >= 2)
        filename = argv[1];

    app_t this;
    this.disp = display_create_by_name(DISPLAY_TYPE);
    this.scrn = screen_create(this.disp);
    this.exec = execution_create(this.scrn, this.disp);

    // create the buffer
    if(filename != NULL) {
        this.buf = fileio_load_buffer(filename);
    } else {
        this.buf = buffer_create();
    }
    this.scrn->add_buffer(this.scrn, this.buf);

    // main loop
    this.disp->main_loop(this.disp);

    this.buf->destroy(this.buf);
    execution_destroy(this.exec);
    this.scrn->destroy(this.scrn);
    this.disp->destroy(this.disp);
    return 0;
}
