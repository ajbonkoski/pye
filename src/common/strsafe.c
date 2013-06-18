#include "strsafe.h"


strsafe_t *strsafe_init(strsafe_t *ss, uint initial_alloc)
{
    ss->alloc = initial_alloc;
    ss->len = 0;
    ss->data = malloc((ss->alloc+1)*sizeof(char));
    return ss;
}

void strsafe_cleanup(strsafe_t *ss)
{
    if(ss->data != NULL)
        free(ss->data);
}

strsafe_t *strsafe_create(uint initial_alloc)
{
    strsafe_t *ss = calloc(1, sizeof(strsafe_t));
    return strsafe_init(ss, initial_alloc);
}

void strsafe_destroy(strsafe_t *ss)
{
    strsafe_cleanup(ss);
    free(ss);
}

void strsafe_require(strsafe_t *ss, uint len)
{
    if(ss->alloc >= len)
        return;

    while(ss->alloc < len)
        ss->alloc *= 2;

    ss->data = realloc(ss->data, (ss->alloc + 1) * sizeof(char));
}

void strsafe_cat(strsafe_t *ss, strsafe_t *ssc)
{
    uint total_len = ss->len + ssc->len;
    strsafe_require(ss, total_len);
    strcpy(ss->data + ss->len, ssc->data);
    ss->len = total_len;
}

void strsafe_cat_char(strsafe_t *ss, char c)
{
    uint total_len = ss->len + 1;
    strsafe_require(ss, total_len);
    ss->data[ss->len++] = c;
    ss->data[ss->len] = '\0';
}
