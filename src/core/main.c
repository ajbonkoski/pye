#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "screen/screen.h"
#include "screen/termio.h"

#define RUNTIME 5*1000*1000

void key_pressed(void *usr, key_event_t *e)
{
    FILE *fh = (FILE *)usr;
    putc(e->key_code, fh);
}

int main(int argc, char *argv[])
{
    screen_t *scrn = screen_create_by_name("terminal");
    FILE *fh = fopen("out", "w");
    scrn->register_kbrd_callback(scrn, key_pressed, fh);

    scrn->main_loop(scrn);

    scrn->destroy(scrn);
    return 0;
}
