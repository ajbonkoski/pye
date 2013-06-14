#include "data_buffer.h"
#include "common/gap_buffer.h"
#include "display/keyboard.h"

#define IMPL_TYPE 0x7f08f1ff

#undef ENABLE_DEBUG
#define ENABLE_DEBUG 1

typedef struct
{
    data_buffer_t *super;

    uint cursor_x;
    uint cursor_y;
    gap_buffer_t *data;

    enum edit_result er;

} data_buffer_internal_t;

// inline functions
static inline data_buffer_internal_t *cast_this(data_buffer_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a data_buffer_internal_t object");
    return (data_buffer_internal_t *)s->impl;
}

static inline bool is_visible(u32 c){ return c >= 0x20 && c <= 0x7e; }


static inline gap_buffer_t *get_line_gb(data_buffer_internal_t *this, uint i) {
    return *(gap_buffer_t **)gap_buffer_get(this->data, i);
}

// forward declarations
static void get_cursor(data_buffer_t *db, uint *x, uint *y);
static void set_cursor(data_buffer_t *db, uint x, uint y);
static void delete_char_left(data_buffer_internal_t *this);
static void delete_char_right(data_buffer_internal_t *this);
static void split_line(data_buffer_internal_t *this);
static void insert(data_buffer_t *db, int c);
static char *get_line_data(data_buffer_t *db, uint i, char *databuf);
static uint line_len(data_buffer_t *db, uint i);
static char get_char_at(data_buffer_t *db, uint x, uint y);
static uint num_lines(data_buffer_t *db);
static enum edit_result get_edit_result(data_buffer_t *db);
static void reset_edit_result(data_buffer_t *db);
static void update_edit_result(data_buffer_internal_t *this, enum edit_result er);
static void destroy(data_buffer_t *db);


static void get_cursor(data_buffer_t *db, uint *x, uint *y)
{
    data_buffer_internal_t *this = cast_this(db);
    *x = this->cursor_x;
    *y = this->cursor_y;
}

static void set_cursor(data_buffer_t *db, uint x, uint y)
{
    data_buffer_internal_t *this = cast_this(db);
    this->cursor_x = x;
    this->cursor_y = y;
    update_edit_result(this, ER_CURSOR);
}

static void delete_char_left(data_buffer_internal_t *this)
{
    if(this->cursor_x == 0) {
        if(this->cursor_y == 0)
            return;

        gap_buffer_t *prev = get_line_gb(this, this->cursor_y-1);
        uint prev_len = gap_buffer_size(prev);
        gap_buffer_t *cur = get_line_gb(this, this->cursor_y);
        gap_buffer_delr(this->data, this->cursor_y);
        gap_buffer_join(prev, cur);

        this->cursor_x = prev_len;
        this->cursor_y -= 1;

        update_edit_result(this, ER_ALL);
        return;
    }

    else {
        gap_buffer_t *gb = get_line_gb(this, this->cursor_y);
        gap_buffer_dell(gb, this->cursor_x);
        this->cursor_x--;

        update_edit_result(this, ER_ALL);
        return;
    }
}

static void delete_char_right(data_buffer_internal_t *this)
{
    gap_buffer_t *gb = get_line_gb(this, this->cursor_y);
    uint gb_size = gap_buffer_size(gb);
    if(this->cursor_x > gb_size)
        return;

    if(this->cursor_x < gb_size) {
        gap_buffer_delr(gb, this->cursor_x);
    }

    // this->cursor_x == gb_size
    else if(this->cursor_y+1 != num_lines(this->super)) {
        gap_buffer_t *cur = get_line_gb(this, this->cursor_y);
        //uint cur_len = gap_buffer_size(cur);
        gap_buffer_t *next = get_line_gb(this, this->cursor_y+1);

        gap_buffer_delr(this->data, this->cursor_y+1);
        gap_buffer_join(cur, next);

        /* this->cursor_x = prev_len; */
        /* this->cursor_y -= 1; */
    }

    update_edit_result(this, ER_ALL);
    return;
}

static void split_line(data_buffer_internal_t *this)
{
    gap_buffer_t *gb = get_line_gb(this, this->cursor_y);
    gap_buffer_t *ngb = gap_buffer_split(gb, this->cursor_x);
    gap_buffer_insert(this->data, this->cursor_y+1, &ngb);

    this->cursor_y += 1;
    this->cursor_x = 0;

    update_edit_result(this, ER_ALL);
    return;
}

static void insert(data_buffer_t *db, int c)
{
    data_buffer_internal_t *this = cast_this(db);

    // "normal" case
    if(is_visible(c)) {
        char ch = (char)c;
        gap_buffer_t *line = get_line_gb(this, this->cursor_y);
        gap_buffer_insert(line, this->cursor_x, &ch);
        this->cursor_x++;
        update_edit_result(this, ER_ALL);
        return;
    }

    // "special" case
    switch(c) {
        case KBRD_BACKSPACE:   return delete_char_left(this);
        case KBRD_DEL:         return delete_char_right(this);
        case '\n':
        case KBRD_ENTER:       return split_line(this);

        default:
            DEBUG("WRN: char '%c' not handled in buffer->input_key()\n", c);
    }

}

static char *get_line_data(data_buffer_t *db, uint i, char *databuf)
{
    data_buffer_internal_t *this = cast_this(db);
    ASSERT(0 <= i && i < num_lines(db), "out of bounds error in data_buffer->get_line_data()");

    gap_buffer_t *line = get_line_gb(this, i);
    return (char *)gap_buffer_to_str(line, databuf);
}

static uint line_len(data_buffer_t *db, uint i)
{
    data_buffer_internal_t *this = cast_this(db);
    ASSERT(0 <= i && i < num_lines(db), "out of bounds error in data_buffer->line_len()");

    gap_buffer_t *line = get_line_gb(this, i);
    return gap_buffer_size(line);
}

static char get_char_at(data_buffer_t *db, uint x, uint y)
{
    //data_buffer_internal_t *this = cast_this(db);
    ASSERT_UNIMPL();
    return 0;
}

static uint num_lines(data_buffer_t *db)
{
    data_buffer_internal_t *this = cast_this(db);
    return gap_buffer_size(this->data);
}

static enum edit_result get_edit_result(data_buffer_t *db)
{
    data_buffer_internal_t *this = cast_this(db);
    return this->er;
}

static void reset_edit_result(data_buffer_t *db)
{
    data_buffer_internal_t *this = cast_this(db);
    this->er = ER_NONE;
}

static void update_edit_result(data_buffer_internal_t *this, enum edit_result er)
{
    if(this->er == ER_ALL || er == ER_ALL) {
        this->er = ER_ALL;
        return;
    }

    if(this->er == ER_CURSOR || er == ER_CURSOR) {
        this->er = ER_CURSOR;
        return;
    }

    return;
}

static void destroy(data_buffer_t *db)
{
    data_buffer_internal_t *this = cast_this(db);

    // free the gap buffers
    size_t numlines = gap_buffer_size(this->data);
    for(size_t i = 0; i < numlines; i++) {
        gap_buffer_t *line = *(gap_buffer_t **)gap_buffer_get(this->data, i);
        gap_buffer_destroy(line);
    }
    gap_buffer_destroy(this->data);

    free(this);
    free(db);
}

data_buffer_internal_t *data_buffer_create_internal(data_buffer_t *db)
{
    data_buffer_internal_t *this = calloc(1, sizeof(data_buffer_internal_t));
    this->super = db;
    this->cursor_x = 0;
    this->cursor_y = 0;
    this->data = gap_buffer_create(sizeof(gap_buffer_t *));
    gap_buffer_t *linebuf = gap_buffer_create(sizeof(char));
    gap_buffer_insert(this->data, 0, &linebuf);
    this->er = ER_NONE;

    return this;
}

data_buffer_t *data_buffer_create(void)
{
    data_buffer_t *db = calloc(1, sizeof(data_buffer_t));
    db->impl = data_buffer_create_internal(db);
    db->impl_type = IMPL_TYPE;

    db->get_cursor = get_cursor;
    db->set_cursor = set_cursor;
    db->insert = insert;
    db->get_line_data = get_line_data;
    db->line_len = line_len;
    db->get_char_at = get_char_at;
    db->num_lines = num_lines;
    db->get_edit_result = get_edit_result;
    db->reset_edit_result = reset_edit_result;
    db->destroy = destroy;

    return db;
}