#ifndef FILEIO_H
#define FILEIO_H

#include "common/common.h"
#include "buffer/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

buffer_t *fileio_load_buffer(const char *filename);
bool fileio_save_buffer(buffer_t *buffer, const char *filename);


#ifdef __cplusplus
}
#endif

#endif
