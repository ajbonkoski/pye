#include "common/common.h"
#include "common/gap_buffer.h"


bool check(gap_buffer_t *gp, const char *s)
{
    size_t sz = gap_buffer_size(gp);
    if(strlen(s) != sz)
        return false;

    for(int i = 0; i < sz; i++) {
        if(s[i] != *(char *)gap_buffer_get(gp, i))
            return false;
    }

    return true;
}

void ANNOUNCE(gap_buffer_t *gb)
{
    char *s = gap_buffer_to_str(gb);
    printf("gap_buffer_t is '%s'\n", s);
    free(s);
}

#define TESTCASE1(method, i, exp) do {                      \
        gap_buffer_ ## method (gb, i);                      \
        ANNOUNCE(gb);                                       \
        ASSERT(check(gb, exp), "failed at gb=='" exp "'");  \
} while(0)

#define TESTCASE2(method, i, c, exp) do {                   \
        char ch = c;                                        \
        gap_buffer_ ## method (gb, i, (void *)&ch);         \
        ANNOUNCE(gb);                                       \
        ASSERT(check(gb, exp), "failed at gb=='" exp "'");  \
} while(0)

int main(int argc, char *argv[])
{
    gap_buffer_t *gb = gap_buffer_create(sizeof(char));

    TESTCASE2(insert, 0, 'a', "a");
    TESTCASE2(insert, 0, 'b', "ba");
    TESTCASE2(insert, 2, 'c', "bac");
    TESTCASE2(insert, 2, 'd', "badc");
    TESTCASE2(insert, 2, 'e', "baedc");
    TESTCASE2(insert, 2, 'f', "bafedc");
    TESTCASE2(insert, 2, 'g', "bagfedc");
    TESTCASE2(insert, 2, 'h', "bahgfedc");
    TESTCASE2(insert, 2, 'i', "baihgfedc");

    TESTCASE1(dell, 2, "bihgfedc");
    TESTCASE1(delr, 2, "bigfedc");

    //TESTCASE1(dell, 0, "bigfedc");  // should be assert error
    TESTCASE1(delr, 0, "igfedc");

    //TESTCASE1(delr, 6, "igfedc");  // should be assert error
    TESTCASE1(dell, 6, "igfed");

    gap_buffer_destroy(gb);

    printf("Test Success!\n");
    return 0;
}
