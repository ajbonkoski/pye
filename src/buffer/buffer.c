#include "buffer.h"
#include "display/display.h"
#include "common/gap_buffer.h"

#define IMPL_TYPE 0x689a9794

typedef struct
{
    buffer_t *super;

    uint cursor_x;
    uint cursor_y;
    gap_buffer_t *data;

} buffer_internal_t;
static buffer_internal_t *cast_this(buffer_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a buffer_internal_t object");
    return (buffer_internal_t *)s->impl;
}

static inline bool is_visible(u32 c){ return c >= 0x20 && c <= 0x7e; }

// forward declarations
static void get_cursor(buffer_t *this, uint *x, uint *y);
static char *get_line_data(buffer_t *this, uint i);
static uint num_lines(buffer_t *this);
static enum edit_result input_key(buffer_t *b, u32 c);
static void destroy(buffer_t *b);

static enum edit_result insert_char(buffer_internal_t *this, char c)
{
    gap_buffer_t *line = *(gap_buffer_t **)gap_buffer_get(this->data, this->cursor_y);
    gap_buffer_insert(line, this->cursor_x, &c);
    this->cursor_x++;
    return ER_ALL;
}

static enum edit_result move_cursor_delta(buffer_internal_t *this, uint dx, uint dy)
{
    uint newx = this->cursor_x + dx;
    uint newy = this->cursor_y + dy;

    uint nlines = gap_buffer_size(this->data);
    if(newy < 0 || newy >= nlines) {
        return ER_NONE;

    } else {

        if(newx < 0)
            newx = 0;

        gap_buffer_t *gb_line = *(gap_buffer_t **)gap_buffer_get(this->data, newy);
        uint linelen = gap_buffer_size(gb_line);
        if(newx > linelen)
            newx = linelen;

        this->cursor_y = newy;
        this->cursor_x = newx;

        return ER_CURSOR;
    }


    //return ER_NONE;
}

static void get_cursor(buffer_t *b, uint *x, uint *y)
{
    buffer_internal_t *this = cast_this(b);
    *x = this->cursor_x;
    *y = this->cursor_y;
}

static char *get_line_data(buffer_t *b, uint i)
{
    buffer_internal_t *this = cast_this(b);
    gap_buffer_t *line = *(gap_buffer_t **)gap_buffer_get(this->data, i);
    return (char *)gap_buffer_to_str(line);
}

static uint num_lines(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);
    return gap_buffer_size(this->data);
}

static enum edit_result input_key(buffer_t *b, u32 c)
{
    buffer_internal_t *this = cast_this(b);

    // normal chars
    if(is_visible(c))
        return insert_char(this, (char)c);

    // handle special chars
    switch(c) {
        case KBRD_ARROW_LEFT:  return move_cursor_delta(this, -1,  0);
        case KBRD_ARROW_RIGHT: return move_cursor_delta(this,  1,  0);
        case KBRD_ARROW_UP:    return move_cursor_delta(this,  0, -1);
        case KBRD_ARROW_DOWN:  return move_cursor_delta(this,  0,  1);

        default:
            DEBUG("WRN: char '%c' not handled in buffer->input_key()\n", c);
    }

    return ER_NONE;
}

static void destroy(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);

    // free the gap buffers
    free(this);
    free(b);
}

buffer_internal_t *buffer_create_internal(buffer_t *b)
{
    buffer_internal_t *this = calloc(1, sizeof(buffer_internal_t));
    this->super = b;
    this->cursor_x = 0;
    this->cursor_y = 0;
    this->data = gap_buffer_create(sizeof(gap_buffer_t *));
    gap_buffer_t *linebuf = gap_buffer_create(sizeof(char));
    gap_buffer_insert(this->data, 0, &linebuf);
    return this;
}

buffer_t *buffer_create(void)
{
    buffer_t *b = calloc(1, sizeof(buffer_t));
    b->impl = buffer_create_internal(b);
    b->impl_type = IMPL_TYPE;
    b->get_cursor = get_cursor;
    b->get_line_data = get_line_data;
    b->num_lines = num_lines;
    b->input_key = input_key;
    b->destroy = destroy;

    return b;
}
