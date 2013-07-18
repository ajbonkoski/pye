#ifndef FILEIO_H
#define FILEIO_H

#include "common/common.h"
#include "buffer/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

// if filename is NULL, create an empty buffer
// if file cannot be opened, create a new buffer with that name
buffer_t *fileio_load_buffer(const char *filename);
bool fileio_save_buffer(buffer_t *buffer, const char *filename);


#ifdef __cplusplus
}
#endif

#endif
