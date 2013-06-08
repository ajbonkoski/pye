#include "timeutil.h"

uint64_t timeutil_utime(void)
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) tv.tv_sec * 1000000 + tv.tv_usec;

}
