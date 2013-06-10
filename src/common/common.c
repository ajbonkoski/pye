#include "common.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

static crash_func_t crash_func = NULL;
static void *usr = NULL;
static bool sigsegv_handler_init = false;
static FILE *debug_file = NULL;
#define DEBUG_FILENAME "debug.log"

static void sigsegv_handler(int signum)
{
    do_crash();
    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

void set_crash_func(crash_func_t crash_func_, void *usr_)
{
    crash_func = crash_func_;
    usr = usr_;

    // register the sigsegv handler
    if(!sigsegv_handler_init) {
        signal(SIGSEGV, sigsegv_handler);
        sigsegv_handler_init = true;
    }
}

bool do_crash()
{
    if(crash_func != NULL) {
        crash_func(usr);
    }

    return false;
}

FILE *common_get_debug_file(void)
{
    if(debug_file == NULL) {
        debug_file = fopen(DEBUG_FILENAME, "w");
        setbuf(debug_file, NULL); // disable buffering
    }

    return debug_file;
}
