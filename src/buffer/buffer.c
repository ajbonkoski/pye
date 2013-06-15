#include "buffer.h"
#include "display/keyboard.h"
#include "common/gap_buffer.h"

#define IMPL_TYPE 0x689a9794

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct
{
    buffer_t *super;

    char *filename;
    data_buffer_t *databuf;

    // formatter callback
    format_func_t formatter_callback;
    void *formatter_usr;

} buffer_internal_t;

// inline functions
static inline buffer_internal_t *cast_this(buffer_t *s)
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


static void move_cursor_delta(buffer_internal_t *this, uint dx, uint dy)
{
    uint x, y;
    data_buffer_t *d = this->databuf;
    d->get_cursor(d, &x, &y);

    uint newx = x + dx;
    uint newy = y + dy;

    uint nlines = d->num_lines(d);
    if(newy < 0 || newy >= nlines) {
        return;

    } else {

        if(newx < 0)
            newx = 0;

        uint linelen = d->line_len(d, newy);
        if(newx > linelen)
            newx = linelen;

        d->set_cursor(d, newx, newy);

        return;
    }
}

static void goto_line_start(buffer_internal_t *this)
{
    uint x, y;
    data_buffer_t *d = this->databuf;
    d->get_cursor(d, &x, &y);
    d->set_cursor(d, 0, y);
}

static void goto_line_end(buffer_internal_t *this)
{
    uint x, y;
    data_buffer_t *d = this->databuf;
    d->get_cursor(d, &x, &y);
    uint ll = d->line_len(d, y);
    d->set_cursor(d, ll, y);
}

static void set_filename(buffer_t *b, const char *filename)
{
    buffer_internal_t *this = cast_this(b);
    if(this->filename != NULL)
        free(this->filename);
    this->filename = strdup(filename);
}

static const char *get_filename(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);
    return this->filename;
}

static void get_cursor(buffer_t *b, uint *x, uint *y)
{
    buffer_internal_t *this = cast_this(b);
    data_buffer_t *d = this->databuf;
    d->get_cursor(d, x, y);
}

static void set_cursor(buffer_t *b, uint x, uint y)
{
    buffer_internal_t *this = cast_this(b);
    data_buffer_t *d = this->databuf;
    d->set_cursor(d, x, y);
}

static char *get_line_data(buffer_t *b, uint i)
{
    buffer_internal_t *this = cast_this(b);
    data_buffer_t *d = this->databuf;
    return d->get_line_data(d, i, NULL);
}

static buffer_line_t *get_line_data_fmt(buffer_t *b, uint i)
{
    buffer_internal_t *this = cast_this(b);
    data_buffer_t *d = this->databuf;

    char *raw = d->get_line_data(d, i, NULL);
    buffer_line_t *bl = NULL;

    if(this->formatter_callback != NULL) {
        bl = this->formatter_callback(this->formatter_usr, raw);

        // if raw is no longer needed, we are responsible for freeing if
        if(bl->data != raw)
            free(raw);
    }

    else {
        bl = calloc(1, sizeof(buffer_line_t));
        bl->styles = NULL;
        bl->regions = NULL;
        bl->data = raw;
    }

    return bl;
}

static void register_formatter(buffer_t *b, format_func_t func, void *usr)
{
    buffer_internal_t *this = cast_this(b);
    this->formatter_callback = func;
    this->formatter_usr = usr;
}

static uint num_lines(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);
    data_buffer_t *d = this->databuf;
    return d->num_lines(d);
}

static enum edit_result input_key(buffer_t *b, u32 c)
{
    buffer_internal_t *this = cast_this(b);
    data_buffer_t *d = this->databuf;

    // handle special chars
    switch(c) {
        case KBRD_ARROW_LEFT:  move_cursor_delta(this, -1,  0); break;
        case KBRD_ARROW_RIGHT: move_cursor_delta(this,  1,  0); break;
        case KBRD_ARROW_UP:    move_cursor_delta(this,  0, -1); break;
        case KBRD_ARROW_DOWN:  move_cursor_delta(this,  0,  1); break;
        case KBRD_CTRL('a'):   goto_line_start(this);           break;
        case KBRD_CTRL('e'):   goto_line_end(this);             break;

        default:  // hande "normal" keys
            d->insert(d, c);
    }

    enum edit_result er = d->get_edit_result(d);
    d->reset_edit_result(d);
    return er;
}

static data_buffer_t *get_data_buffer(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);
    return this->databuf;
}

static void destroy(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);

    this->databuf->destroy(this->databuf);
    free(this);
    free(b);
}

buffer_internal_t *buffer_create_internal(buffer_t *b)
{
    buffer_internal_t *this = calloc(1, sizeof(buffer_internal_t));
    this->super = b;
    this->databuf = data_buffer_create();
    this->formatter_callback = NULL;
    this->formatter_usr = NULL;
    return this;
}

buffer_t *buffer_create(void)
{
    buffer_t *b = calloc(1, sizeof(buffer_t));
    b->impl = buffer_create_internal(b);
    b->impl_type = IMPL_TYPE;
    b->set_filename = set_filename;
    b->get_filename = get_filename;
    b->get_cursor = get_cursor;
    b->set_cursor = set_cursor;
    b->get_line_data = get_line_data;
    b->get_line_data_fmt = get_line_data_fmt;
    b->register_formatter = register_formatter;
    b->num_lines = num_lines;
    b->input_key = input_key;
    b->destroy = destroy;

    b->get_data_buffer = get_data_buffer;

    return b;
}
