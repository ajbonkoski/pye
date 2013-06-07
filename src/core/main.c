#include <signal.h>

#include "common/common.h"
#include "common/timeutil.h"
#include "screen/screen.h"

bool is_running = true;

int main(int argc, char *argv[])
{
    screen_t *scrn = screen_create_by_name("terminal");

    sleep(5);

    scrn->destroy(scrn);
    return 0;
}
