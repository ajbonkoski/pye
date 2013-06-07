#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

inline int32_t  timeutil_usleep(__useconds_t us) { return usleep(us);      }
inline uint32_t timeutil_msleep(__useconds_t ms) { return usleep(ms*1000); }
inline uint32_t timeutil_sleep(unsigned int s)   { return sleep(s);        }

#ifdef __cplusplus
}
#endif

#endif
