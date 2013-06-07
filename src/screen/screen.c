#include "screen.h"

#include <string.h>

#include "screen_terminal.h"

screen_t *screen_create_by_name(const char *name)
{
    #define CHECK(s, f) if(strcmp(s, name) == 0) return f();

    CHECK("terminal", screen_terminal_create);

    ERROR("Failed to recognize '%s' in screen_create_by_name\n", name);
    ASSERT_FAIL("Fatal Error");

    #undef CHECK
}
