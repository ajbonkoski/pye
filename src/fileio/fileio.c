#include "fileio.h"

buffer_t *fileio_load_buffer(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        ERROR("Failed to open file '%s'\n", filename);
        return NULL;
    }

    buffer_t *b = buffer_create();
    b->set_filename(b, filename);

    int c;
    while(true) {
        c = getc(f);
        if(c == EOF)
            break;

        if(c == '\n')
            b->endline(b);
        else
            b->insert(b, (char)c);
    }

    fclose(f);

    b->set_cursor(b, 0, 0); // start cursor at the top
    return b;
}

bool fileio_save_buffer(buffer_t *buffer, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if(f == NULL) {
        ERROR("Failed to open file '%s'\n", filename);
        return false;
    }

    uint numlines = buffer->num_lines(buffer);
    for(uint i = 0; i < numlines; i++) {
        char *line = buffer->get_line_data(buffer, i);
        if(i != numlines-1 || strlen(line) != 0)
            fprintf(f, "%s\n", line);
        free(line);
    }

    fclose(f);

    return true;
}

