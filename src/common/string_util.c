#include "string_util.h"

char *__string_util_concat_varargs_private(const char *first, ...)
{
    uint len = 0;

    // get the total length (for the allocation)
    {
        va_list args;
        va_start(args, first);
        const char *arg = first;
        while(arg != NULL) {
            len += strlen(arg);
            arg = va_arg(args, const char *);
        }
        va_end(args);
    }

    // write the string
    char *str = malloc(len*sizeof(char) + 1);
    char *ptr = str;
    {
        va_list args;
        va_start(args, first);
        const char *arg = first;
        while(arg != NULL) {
            while(*arg)
                *ptr++ = *arg++;
            arg = va_arg(args, const char *);
        }
        *ptr = '\0';
        va_end(args);
    }

    return str;
}
