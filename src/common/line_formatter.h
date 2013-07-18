#ifndef LINE_FORMATTER_H
#define LINE_FORMATTER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct line_formatter line_formatter_t;

line_formatter_t *line_formatter_create(size_t linesize, char fill);
void line_formatter_destroy(line_formatter_t *this);

const char *line_formatter_cstr(line_formatter_t *this);
void line_formatter_add_str(line_formatter_t *this, const char *s, size_t fieldsize, char fill);

#ifdef __cplusplus
}
#endif

#endif
