#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "screen/screen.h"
#include "screen/termio.h"

#define KEY_CTRL_C 3

void key_pressed1(void *usr, key_event_t *e)
{
    FILE *fh = (FILE *)usr;
    fprintf(fh, "%d\n", e->key_code);
}

void key_pressed2(void *usr, key_event_t *e)
{
    fprintf(stderr, "%c", e->key_code);
}

void ctrl_c_listener(void *usr, key_event_t *e)
{
    screen_t *scrn = (screen_t *)usr;
    if(e->key_code == KEY_CTRL_C) {
        scrn->main_quit(scrn);
    }
}

int main(int argc, char *argv[])
{
    screen_t *scrn = screen_create_by_name("terminal");
    FILE *fh = fopen("out", "w");

    scrn->register_kbrd_callback(scrn, key_pressed1, fh);
    scrn->register_kbrd_callback(scrn, key_pressed2, fh);
    scrn->register_kbrd_callback(scrn, ctrl_c_listener, scrn);

    scrn->main_loop(scrn);

    scrn->destroy(scrn);
    return 0;
}
