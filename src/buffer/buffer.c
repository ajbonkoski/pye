#include "buffer.h"
#include "display/keyboard.h"
#include "common/varray.h"

#define IMPL_TYPE 0x689a9794

//#undef ENABLE_DEBUG
//#define ENABLE_DEBUG 1

typedef struct
{
    uint start;
    uint end;
    uint style;

} buffer_region_t;

static inline void buffer_region_destroy(buffer_region_t *this)
{
    free(this);
}

typedef struct
{
    buffer_t *super;

    char *filename;
    data_buffer_t *databuf;

    // formatter callback
    format_func_t formatter_callback;
    void *formatter_usr;

    // marks
    bool mark_set;
    uint mark_x;
    uint mark_y;

    // highlight
    varray_t *highlight_regions;  // dynamic array of buffer_region_t's

} buffer_internal_t;

// inline functions
static inline buffer_internal_t *cast_this(buffer_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a buffer_internal_t object");
    return (buffer_internal_t *)s->impl;
}

static inline bool is_visible(u32 c){ return c >= 0x20 && c <= 0x7e; }

// forward declarations
static bool get_mark(buffer_t *b, uint *x, uint *y);
static void set_mark(buffer_t *b, uint x, uint y);
static void clear_mark(buffer_t *b);
static void goto_line_start(buffer_t *this);
static void goto_line_end(buffer_t *this);
static void get_cursor(buffer_t *this, uint *x, uint *y);
static char *get_line_data(buffer_t *this, uint i);
static uint num_lines(buffer_t *this);
static enum edit_result input_key(buffer_t *b, u32 c);
static void enable_highlight(buffer_t *b, uint start, uint end, uint style);
static void clear_highlight(buffer_t *b);
static void destroy(buffer_t *b);


static bool get_mark(buffer_t *b, uint *x, uint *y)
{
    buffer_internal_t *this = cast_this(b);
    *x = this->mark_x;
    *y = this->mark_y;
    return this->mark_set;
}

static void set_mark(buffer_t *b, uint x, uint y)
{
    buffer_internal_t *this = cast_this(b);
    this->mark_set = true;
    this->mark_x = x;
    this->mark_y = y;
}

static void clear_mark(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);
    this->mark_set = false;
}


static void move_cursor_delta(buffer_internal_t *this, uint dx, uint dy)
{
    uint x, y;
    data_buffer_t *d = this->databuf;
    d->get_cursor(d, &x, &y);

    int newx = (int)x + (int)dx;
    int newy = (int)y + (int)dy;
    uint nlines = d->num_lines(d);
    if(newy < 0 || newy >= nlines)
        return;

    uint linelen = d->line_len(d, newy);

    if(newx < 0) {
        if(newy <= 0) {
            newx = 0;
            newy = 0;
        } else {
            newy -= 1;
                linelen = d->line_len(d, newy);
                newx = linelen;
        }
    }

    if(newx > linelen) {
        if(newy + 1 < nlines) {
            newx = 0;
            newy += 1;
        } else {
            newx = linelen;
            newy = nlines-1;
        }
    }

    d->set_cursor(d, newx, newy);

    return;
}

static void goto_line_start(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);

    uint x, y;
    data_buffer_t *d = this->databuf;
    d->get_cursor(d, &x, &y);
    d->set_cursor(d, 0, y);
}

static void goto_line_end(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);

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
        buffer_line_t *pre_bl = calloc(1, sizeof(buffer_line_t));
        pre_bl->styles = NULL;

        // convert our representation into the interfaces
        pre_bl->regions = varray_create();
        buffer_region_t *br;
        varray_iter(br, this->highlight_regions) {
            buffer_line_region_t *blr = calloc(1, sizeof(buffer_line_region_t));
            blr->start_index = br->start;
            blr->length = br->end - br->start;
            blr->style_id = br->style;
            varray_add(pre_bl->regions, blr);
        }

        pre_bl->data = raw;
        bl = this->formatter_callback(this->formatter_usr, pre_bl);

    }

    else {
        bl = calloc(1, sizeof(buffer_line_t));
        bl->styles = varray_create();
        bl->regions = varray_create();
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

static void enable_highlight(buffer_t *b, uint start, uint end, uint style)
{
    buffer_internal_t *this = cast_this(b);

    // this is a simple, stupid implementation it is highly
    // inefficient and WILL fail in under some common situations
    uint sz = varray_size(this->highlight_regions);
    buffer_region_t *br;
    uint i;

    for(i = 0; i < sz; i++) {
        br = varray_get(this->highlight_regions, i);
        if(br->start >= start)
            break;
    }

    // some aliases
    varray_t *va = this->highlight_regions;  // a quick alias

    // style is NONE, then remove it if possible
    if(style == HIGHLIGHT_STYLE_NONE) {
        if(br->start != start || br->end != end) {
            DEBUG("WRN: tried to remove highlighting of disjoint region: unsupported, ignoring...");
            return;
        }

        // free the element
        buffer_region_destroy(br);

        // shift the array
        for(; i < sz-1; i++) {
            varray_set(va, i, varray_get(va, i+1));
        }
        varray_shrink(va, sz-1, NULL);

        return;
    }

    // an enabled style region
    else {

        // begin by shifting the array
        DEBUG("buffer->enable_highlight(): before va->size == %d\n", varray_size(va));
        varray_grow(va, sz+1, NULL);
        DEBUG("buffer->enable_highlight(): after va->size == %d\n", varray_size(va));
        for(uint j = sz-1; j >= i && i >= 0 && sz > 0; j--) {
            varray_set(va, j+1, varray_get(va, j));
            if(j == 0)
                break;
        }

        // set the new buffer_region_t
        br = calloc(1, sizeof(buffer_region_t));
        br->start = start;
        br->end = end;
        br->style = style;
        varray_set(va, i, br);

        return;
    }
}

static void clear_highlight(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);
    varray_shrink(this->highlight_regions, 0,
                  (void (*)(void *))buffer_region_destroy);
}

static void destroy(buffer_t *b)
{
    buffer_internal_t *this = cast_this(b);

    this->databuf->destroy(this->databuf);
    varray_map(this->highlight_regions, (void (*)(void *))buffer_region_destroy);
    varray_destroy(this->highlight_regions);

    free(this);
    free(b);
}

static buffer_internal_t *buffer_create_internal(buffer_t *b)
{
    buffer_internal_t *this = calloc(1, sizeof(buffer_internal_t));
    this->super = b;
    this->databuf = data_buffer_create();
    this->formatter_callback = NULL;
    this->formatter_usr = NULL;

    this->mark_set = false;
    this->mark_x = 0;
    this->mark_y = 0;

    this->highlight_regions = varray_create();

    return this;
}

buffer_t *buffer_create(void)
{
    buffer_t *b = calloc(1, sizeof(buffer_t));
    b->impl = buffer_create_internal(b);
    b->impl_type = IMPL_TYPE;

    b->get_mark = get_mark;
    b->set_mark = set_mark;
    b->clear_mark = clear_mark;

    b->set_filename = set_filename;
    b->get_filename = get_filename;
    b->get_cursor = get_cursor;
    b->set_cursor = set_cursor;
    b->goto_line_start = goto_line_start;
    b->goto_line_end = goto_line_end;
    b->get_line_data = get_line_data;
    b->get_line_data_fmt = get_line_data_fmt;
    b->register_formatter = register_formatter;
    b->num_lines = num_lines;
    b->input_key = input_key;
    b->enable_highlight = enable_highlight;
    b->clear_highlight = clear_highlight;
    b->destroy = destroy;

    b->get_data_buffer = get_data_buffer;

    return b;
}
