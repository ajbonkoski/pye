#include "common.h"

static void (*shutdown_func)(void *usr);
static void *usr;

void set_crash_func(void (*shutdown_func_)(void *usr), void *usr_)
{
    shutdown_func = shutdown_func_;
    usr = usr_;
}

bool do_crash()
{
    shutdown_func(usr);
    return false;
}
