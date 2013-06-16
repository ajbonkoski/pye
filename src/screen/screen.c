#include "screen.h"
#include "fileio/fileio.h"

#define IMPL_TYPE 0x7c870c4b
#define BLANK ' '
#define FILENAME "testfile.txt"

//#undef  ENABLE_DEBUG
//#define ENABLE_DEBUG 1

typedef struct
{
    screen_t *super;

    display_t *display;
    uint mb_y;
    uint disp_width;

    varray_t *buffers;
    buffer_t *cb;  /* the current buffer */

    // key handler data
    key_event_func_t key_callback;
    void *key_usr;

    // buf added handler data
    buf_event_func_t buf_callback;
    void *buf_usr;

} screen_internal_t;
static screen_internal_t *cast_this(screen_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a screen_internal_t object");
    return (screen_internal_t *)s->impl;
}

// forward declarations
static void register_kbrd_callback(screen_t *scrn, key_event_func_t f, void *usr);
static void register_buf_added_callback(screen_t *scrn, buf_event_func_t f, void *usr);

static uint add_buffer(screen_t *scrn, buffer_t *buffer);
static varray_t *list_buffers(screen_t *scrn);
static buffer_t *get_buffer(screen_t *scrn, uint id);
static void set_active_buffer(screen_t *scrn, uint id);
static buffer_t *get_active_buffer(screen_t *scrn);
static void write_mb(screen_t *scrn, const char *str);
static void destroy(screen_t *scrn);
static void update_all(screen_internal_t *this);
static void refresh(screen_t *scrn);

static void register_kbrd_callback(screen_t *scrn, key_event_func_t f, void *usr)
{
    screen_internal_t *this = cast_this(scrn);
    this->key_callback = f;
    this->key_usr = usr;
}

static void register_buf_added_callback(screen_t *scrn, buf_event_func_t f, void *usr)
{
   screen_internal_t *this = cast_this(scrn);
   this->buf_callback = f;
   this->buf_usr = usr;
}

static uint add_buffer(screen_t *scrn, buffer_t *buffer)
{
    screen_internal_t *this = cast_this(scrn);

    // this static var is used to detect cyclic calls
    // that could arise due to the use of the
    // "buffer added" event handler
    static bool running = false;
    ASSERT(!running, "Detected cyclic calls to add_buffer");
    running = true;

    varray_add(this->buffers, buffer);
    uint id = varray_size(this->buffers) - 1;
    bool needs_update = false;

    if(this->cb == NULL) {
        this->cb = buffer;
        needs_update = true;
    }

    // call the handler if needed
    if(this->buf_callback != NULL) {
        this->buf_callback(this->buf_usr, id);
    }

    if(needs_update)
        update_all(this);

    running = false;
    return id;
}

static varray_t *list_buffers(screen_t *scrn)
{
    screen_internal_t *this = cast_this(scrn);
    return this->buffers;
}

static buffer_t *get_buffer(screen_t *scrn, uint id)
{
    screen_internal_t *this = cast_this(scrn);
    ASSERT(id >= 0 && id < varray_size(this->buffers), "Index out-of-bounds error");
    return varray_get(this->buffers, id);
}

static void set_active_buffer(screen_t *scrn, uint id)
{
    screen_internal_t *this = cast_this(scrn);
    ASSERT(id >= 0 && id < varray_size(this->buffers), "Index out-of-bounds error");
    this->cb = varray_get(this->buffers, id);
}

static buffer_t *get_active_buffer(screen_t *scrn)
{
    screen_internal_t *this = cast_this(scrn);
    return this->cb;
}

static void write_mb(screen_t *scrn, const char *str)
{
    screen_internal_t *this = cast_this(scrn);

    // step 1: start by saving the current cursor position
    uint x, y;
    this->display->get_cursor(this->display, &x, &y);

    // step 2: move cursor to the start of the mb line
    this->display->set_cursor(this->display, 0, this->mb_y);

    // step 3: prepare display string and write it
    char *buf = malloc(this->disp_width * sizeof(char));
    bool at_end = false;
    for(int i = 0; i < this->disp_width; i++) {
        if(str == NULL || str[i] == '\0')
            at_end = true;
        if(!at_end) {
            buf[i] = str[i];
        } else {
            buf[i] = BLANK;
        }
    }

    this->display->write(this->display, buf, this->disp_width, -1);
    free(buf);

    // step 4: restore cursor position
    this->display->set_cursor(this->display, x, y);
}

static void destroy(screen_t *scrn)
{
    screen_internal_t *this = cast_this(scrn);

    buffer_t *buffer;
    varray_iter(buffer, this->buffers) {
        buffer->destroy(buffer);
    }
    varray_destroy(this->buffers);

    free(this);
    free(scrn);
}

static void display_write_line(screen_internal_t *this, buffer_line_t *line, int lineno)
{
    this->display->set_cursor(this->display, 0, lineno);

    this->display->set_styles(this->display, line->styles);

    uint index = 0;
    uint line_len = strlen(line->data);
    uint num_styles = varray_size(line->styles);
    buffer_line_region_t *r;

    varray_iter(r, line->regions) {

        // do validation first
        if(r->start_index < index) {
            ERROR("expected a region start_index >= %d, but got %d. "
                  "Skipping region\n", index, r->start_index);
            continue;
        }

        if(r->start_index >= line_len) {
            ERROR("expected a region start_index < line length of %d, but got %d. "
                  "Skipping region\n", line_len, r->start_index);
            continue;
        }

        if(r->start_index + r->length > line_len) {
            ERROR("expected a region end index <= line length of %d, but got %d. "
                  "Skipping region\n", line_len, r->start_index + r->length);
            continue;
        }

        if(r->style_id >= num_styles) {
            ERROR("expected a region style_id < %d, but got %d. "
                  "Skipping region\n", num_styles, r->style_id);
            continue;
        }

        // display the "un-stylized"
        uint unstyled_size = r->start_index - index;
        if(unstyled_size > 0) {
            this->display->write(this->display,
                                 line->data + index,
                                 unstyled_size, -1);
        }

        // display this region
        this->display->write(this->display,
                                 line->data + r->start_index,
                                 r->length, r->style_id);

        // update the index
        index = r->start_index + r->length;
    }

    // we might have some residual line data after the regions
    uint left = line_len - index;
    if(index < line_len && left > 0) {
        DEBUG("Handling the residuals: data='%s'\n", line->data + index);
        this->display->write(this->display,
                             line->data + index,
                             left, -1);
    }

    this->display->remove_styles(this->display);


    // write some blanks (to the end of the line)
    uint x, y, w, h;
    this->display->get_size(this->display, &w, &h);
    this->display->get_cursor(this->display, &x, &y);
    this->display->write(this->display, NULL, w-(x+1), -1);
}

static void update_cursor(screen_internal_t *this)
{
    uint x, y;
    this->cb->get_cursor(this->cb, &x, &y);
    this->display->set_cursor(this->display, x, y);
}

static void update_all(screen_internal_t *this)
{
    DEBUG("inside screen->update_all(): entering\n");

    uint w, h;
    this->display->get_size(this->display, &w, &h);

    // write the lines in the buffer_t
    uint numlines = this->cb->num_lines(this->cb);
    int i;
    for(i = 0; i < numlines && i < h-1; i++) {
        buffer_line_t *line = this->cb->get_line_data_fmt(this->cb, i);
        display_write_line(this, line, i);
        buffer_line_destroy(line);
    }

    // clear the remainer of the screen
    for(; i < h-1; i++) {
        this->display->write(this->display, NULL, w, -1);
    }

    DEBUG("inside screen->update_all: updating cursor()\n");
    update_cursor(this);

    DEBUG("inside screen->update_all: leaving\n");
}

static void refresh(screen_t *scrn)
{
    update_all(cast_this(scrn));
}

static bool key_handler(void *usr, key_event_t *e)
{
    screen_internal_t *this = (screen_internal_t *)usr;
    bool ret = false;

    const int BUFSZ = 256;
    char buffer[BUFSZ];

    DEBUG("inside key_handler(): entering\n");

    // allow any registered handler to override functionality
    if(this->key_callback) {
        bool is_handled = this->key_callback(this->key_usr, e);
        if(is_handled) {
            ret = true;
            goto done;
        }
    }

    u32 c = e->key_code;
    //char ch = (char)c;

    uint w, h, x, y;
    this->display->get_size(this->display, &w, &h);
    this->display->get_cursor(this->display, &x, &y);

    // message buffer keystrokes
    if(c == KBRD_CTRL('r')) {
        sprintf(buffer, "|w=%d h=%d|", w, h);
        write_mb(this->super, buffer);
    }

    else if(c == KBRD_CTRL('l')) {
        sprintf(buffer, "|x=%d y=%d|", x, y);
        write_mb(this->super, buffer);
    }

    else if(c == KBRD_CTRL('g')) {
        write_mb(this->super, NULL);
    }

    else if(c == KBRD_CTRL('f')) {
        screen_t *s = this->super;
        buffer_t *b = fileio_load_buffer(FILENAME);
        if(b == NULL) {
            snprintf(buffer, BUFSZ, "failed to read: %s", FILENAME);
            write_mb(s, buffer);
        } else {
            s->set_active_buffer(s, s->add_buffer(s, b));
            update_all(this);
        }
    }

    else if(c == KBRD_CTRL('w')) {
        const char *filename = this->cb->get_filename(this->cb);
        bool success = fileio_save_buffer(this->cb, filename);
        if(!success) {
            snprintf(buffer, BUFSZ, "failed to save: %s", filename);
        } else {
            snprintf(buffer, BUFSZ, "successfully saved: %s", filename);
        }
        write_mb(this->super, buffer);
    }

    // "normal" keystrokes - reroute these to the buffer
    else {
        if(this->cb != NULL) {
            enum edit_result er = this->cb->input_key(this->cb, c);
            switch(er) {
                case ER_CURSOR: update_cursor(this);  break;
                case ER_ALL:    update_all(this);     break;
                case ER_NONE:   /* noop */            break;
                default:
                    DEBUG("unrecognized enum value for edit_result: %d\n", er);
            }
        }
    }

 done:
    DEBUG("inside key_handler(): leaving\n");
    return ret;
}

static screen_internal_t *screen_create_internal(screen_t *s, display_t *disp)
{
    screen_internal_t *this = calloc(1, sizeof(screen_internal_t));
    this->super = s;
    this->display = disp;
    uint w, h;
    this->display->get_size(this->display, &w, &h);
    this->display->register_kbrd_callback(this->display, key_handler, this);
    this->mb_y = h-1;
    this->disp_width = w;
    this->buffers = varray_create();

    this->buf_callback = NULL;
    this->buf_usr = NULL;

    return this;
}

screen_t *screen_create(display_t *disp)
{
    screen_t *s = calloc(1, sizeof(screen_t));
    s->impl = screen_create_internal(s, disp);
    s->impl_type = IMPL_TYPE;

    s->register_kbrd_callback = register_kbrd_callback;
    s->register_buf_added_callback = register_buf_added_callback;
    s->add_buffer = add_buffer;
    s->list_buffers = list_buffers;
    s->get_buffer = get_buffer;
    s->set_active_buffer = set_active_buffer;
    s->get_active_buffer = get_active_buffer;
    s->write_mb = write_mb;
    s->destroy = destroy;
    s->refresh = refresh;

    return s;
}
