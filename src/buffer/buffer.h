#ifndef BUFFER_H
#define BUFFER_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct buffer buffer_t;
struct buffer
{
    void (*input_key)(buffer_t *this, u32 c);
    void (*destroy)(buffer_t *this);
};

buffer_t *buffer_create(void);

#ifdef __cplusplus
}
#endif

#endif
