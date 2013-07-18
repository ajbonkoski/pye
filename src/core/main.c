#include "common/common.h"
#include "display/display.h"
#include "screen/screen.h"
#include "fileio/fileio.h"
#include "dirman/dirman.h"
#include "exec/execution.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

//#define DISPLAY_TYPE "terminal"
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
    dirman_t *dm = dirman_create(argv[0]);
    common_set_log_path(dirman_get_log_dir(dm));
    common_redirect_stderr();

    // parse args
    const char *filename = NULL;
    if(argc >= 2)
        filename = argv[1];

    app_t this;
    this.disp = display_create_by_name(DISPLAY_TYPE);
    this.scrn = screen_create(this.disp);

    // make the scripting execution context
    this.exec = execution_create(dirman_get_pye_script_dir(dm), this.scrn, this.disp);

    // create the buffer
    this.buf = fileio_load_buffer(filename); // if filename == NULL, create an empty buffer
    this.scrn->add_buffer(this.scrn, this.buf);


    // main loop
    this.disp->main_loop(this.disp);

    //this.buf->destroy(this.buf);

    execution_destroy(this.exec);
    this.scrn->destroy(this.scrn);
    this.disp->destroy(this.disp);

    dirman_destroy(dm);
    return 0;
}
