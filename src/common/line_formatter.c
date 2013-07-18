#include "line_formatter.h"

struct line_formatter
{
    char *line;
    size_t linesize;
    size_t index;
    char fill;
};

static void clear_line(line_formatter_t *this)
{
    for(size_t i = 0; i < this->linesize; i++){
        this->line[i] = this->fill;
    }
    this->line[this->linesize] = '\0';
    this->index = 0;
}

line_formatter_t *line_formatter_create(size_t linesize, char fill)
{
    line_formatter_t *this = calloc(1, sizeof(line_formatter_t));
    this->line = malloc((linesize+1) * sizeof(char));
    this->linesize = linesize;
    this->index = 0;
    this->fill = fill;
    clear_line(this);

    return this;
}

void line_formatter_destroy(line_formatter_t *this)
{
    free(this->line);
    free(this);
}

const char *line_formatter_cstr(line_formatter_t *this)
{
    return this->line;
}

void line_formatter_add_str(line_formatter_t *this, const char *s, size_t fieldsize, char fill)
{
    size_t linesize = this->linesize;
    if(!fill) fill = this->fill;
    if(s == NULL) s = "";  // simplify the branch logic in the loop

    for(size_t i = 0; i < fieldsize && this->index < linesize; i++, this->index++) {
        if(*s) {
            this->line[this->index] = *s;
            s++;
        } else {
            this->line[this->index] = fill;
        }
    }
}
