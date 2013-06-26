#include "common.h"
#include "common/string_util.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

static crash_func_t crash_func = NULL;
static void *usr = NULL;
static bool sigsegv_handler_init = false;
static const char *log_path = NULL;
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
        const char *path = DEBUG_FILENAME;

        char *path_str = NULL;
        if(log_path != NULL) {
            path = path_str = string_util_concat(log_path, "/", DEBUG_FILENAME);
        }

        debug_file = fopen(path, "w");
        setbuf(debug_file, NULL); // disable buffering

        // cleanup
        if(path_str != NULL)
            free(path_str);
    }

    return debug_file;
}

void common_set_log_path(const char *path)
{
    log_path = path;
}

void common_redirect_stderr(void)
{
    FILE *debug_file = common_get_debug_file();
    stderr = debug_file;
}
