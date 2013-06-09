#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "screen/screen.h"
#include "screen/termio.h"


bool key_pressed(void *usr, key_event_t *e)
{
    printf("%c(%d)\n", e->key_code, e->key_code);
    fflush(stdout);

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
