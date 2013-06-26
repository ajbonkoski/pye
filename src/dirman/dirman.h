#ifndef DIRMAN_H
#define DIRMAN_H

#include "common/common.h"

/* A manager for important directories */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dirman dirman_t;

dirman_t *dirman_create(const char *executable_path);
void dirman_destroy(dirman_t *this);

const char *dirman_get_pye_script_dir(dirman_t *this);
const char *dirman_get_log_dir(dirman_t *this);

#ifdef __cplusplus
}
#endif

#endif
