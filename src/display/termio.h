#ifndef TERMIO_H
#define TERMIO_H

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ttopen(void);
bool ttclose(void);
int ttputc(int c);
void ttflush(void);
int ttgetc(void);
int typahead(void);

#ifdef __cplusplus
}
#endif

#endif
