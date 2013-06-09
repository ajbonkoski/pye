#include "common.h"

crash_func_t crash_func;
static void *usr;

void set_crash_func(crash_func_t crash_func_, void *usr_)
{
    crash_func = crash_func_;
    usr = usr_;
}

bool do_crash()
{
    crash_func(usr);
    return false;
}
