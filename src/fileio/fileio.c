#include "fileio.h"

buffer_t *fileio_load_buffer(const char *filename)
{
    buffer_t *b = buffer_create();

    if(filename == NULL)
        goto ret;

    b->set_filename(b, filename);

    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        ERROR("Failed to open file '%s'... Creating new file\n", filename);
        goto ret;
    }

    int c;
    while(true) {
        c = getc(f);
        if(c == EOF)
            break;

        b->input_key(b, c);
    }

    fclose(f);

 ret:
    b->set_cursor(b, 0, 0); // start cursor at the top
    b->set_is_edited(b, false); // clear the 'is_edited' because we just loaded it
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

