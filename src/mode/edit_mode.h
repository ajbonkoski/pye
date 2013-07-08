#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include "common/common.h"
#include "display/keyboard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct edit_mode edit_mode_t;
struct edit_mode {

    void *impl;
    uint impl_type;

    void (*begin_mode)(edit_mode_t *this, va_list args);
    bool (*on_key)(edit_mode_t *this, key_event_t *key);
    void (*destroy)(edit_mode_t *this);
};

edit_mode_t *edit_mode_create_factory(const char *typename);

#ifdef __cplusplus
}
#endif

#endif
