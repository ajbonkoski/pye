#include "string_lib.h"

inline static string_t *string_init(string_t *this, size_t size)
{
    this->len = size;
    this->str = (char *) malloc((this->len+1) * sizeof(char));
    this->str[0] = '\0';
    return this;
}

string_t *string_alloc(size_t initialsize)
{
    string_t *s = (string_t *) malloc(sizeof(string_t));
    return string_init(s, initialsize);
}

string_t *string_copy(string_t *this, string_t *other)
{
    if(this == NULL)
        this = string_alloc(other->len);

    if(this->str == NULL)
        string_init(this, other->len);

    // need to reallocate?
    if(this->len < other->len || this->str == NULL){
        this->str = realloc(this->str, other->len+1);
        this->len = other->len;
    }

    strcpy(this->str, other->str);
    return this;
}

void string_free(string_t *this)
{
    if(this->str != NULL) free(this->str);
    free(this);
}

// concats 'other' onto 'this', returning 'this'
string_t *string_concat(string_t *this, string_t *other)
{
    assert(this != NULL && other != NULL);
    assert(this->str != NULL && other->str != NULL);

    size_t totalsz = this->len + other->len + 1;
    this->str = realloc(this->str, totalsz);
    strcpy(this->str + this->len, other->str);

    return this;
}
