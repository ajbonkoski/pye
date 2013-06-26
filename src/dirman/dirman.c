#include "dirman.h"

struct dirman
{
    const char *executable_path;
    char *pye_script_dir;
    char *log_dir;
};

#define PYE_SUFFIX "/../src"
#define LOG_SUFFIX "/../log"

static inline char *make_path(const char *orig_path, uint prefix_len, const char *suffix)
{
    char *path = malloc((prefix_len+strlen(suffix)+1) * sizeof(char));
    strncpy(path, orig_path, prefix_len);
    strcat(path+prefix_len, suffix);
    return path;
}

dirman_t *dirman_create(const char *executable_path)
{
    dirman_t *this = calloc(1, sizeof(dirman_t));
    this->executable_path = executable_path;

    uint len = strlen(executable_path);
    uint slash_loc = 0;
    for(uint i = 0; i < len; i++)
        if(executable_path[i] == '/')
            slash_loc = i;

    this->pye_script_dir = make_path(executable_path, slash_loc, PYE_SUFFIX);
    this->log_dir = make_path(executable_path, slash_loc, LOG_SUFFIX);

    return this;
}

void dirman_destroy(dirman_t *this)
{
    if(this->pye_script_dir != NULL)
        free(this->pye_script_dir);

    if(this->log_dir != NULL)
        free(this->log_dir);
}

const char *dirman_get_pye_script_dir(dirman_t *this)
{
    return this->pye_script_dir;
}

const char *dirman_get_log_dir(dirman_t *this)
{
    return this->log_dir;
}
