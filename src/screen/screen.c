#include "screen.h"
#include "screen_terminal.h"

screen_t *screen_create_by_name(const char *name)
{
    #define CHECK(s, f) if(strcmp(s, name) == 0) return f();

    CHECK("terminal", screen_terminal_create);
    ASSERT_FAIL("Failed to recognize '%s' in screen_create_by_name\n", name);

    #undef CHECK
}
