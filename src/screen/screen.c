#include "screen.h"
#include "fileio/fileio.h"

#define IMPL_TYPE 0x7c870c4b
#define BLANK ' '

//#undef  ENABLE_DEBUG
//#define ENABLE_DEBUG 1

#define FOOTER_SIZE 2
#define MB_RESPONSE_SIZE 200
#define DISPLAY_SIZE(w, h) \
    uint w, h; this->display->get_size(this->display, &w, &h)

static inline bool is_visible(u32 c){ return c >= 0x20 && c <= 0x7e; }

typedef struct
{
    screen_t *super;

    display_t *display;
    uint mb_y;
    uint disp_width;

    varray_t *buffers;
    buffer_t *cb;        /* the current buffer */
    uint cb_viewport_y;  /* the top line of the current buffer's viewport */

    // key handler data
    key_event_func_t key_callback;
    void *key_usr;

    // buf added handler data
    buf_event_func_t buf_callback;
    void *buf_usr;

    // message box state
    bool mb_mode;
    mb_response_func_t mb_response_func;
    void *mb_response_usr;
    char *mb_question;
    char mb_response[MB_RESPONSE_SIZE];
    uint mb_response_index;

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
static void mb_write(screen_t *scrn, const char *str);
static void mb_ask(screen_t *scrn, const char *str, mb_response_func_t func, void *usr);
static void mb_ask_rewrite(screen_internal_t *this);
static void update_sb(screen_internal_t *this);
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
        this->cb_viewport_y = 0;
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
    this->cb_viewport_y = 0;
}

static buffer_t *get_active_buffer(screen_t *scrn)
{
    screen_internal_t *this = cast_this(scrn);
    return this->cb;
}

static void mb_write(screen_t *scrn, const char *str)
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

static void mb_ask(screen_t *scrn, const char *str, mb_response_func_t func, void *usr)
{
    screen_internal_t *this = cast_this(scrn);
    ASSERT(!this->mb_mode, "cannot call mb_ask while already pending!");

    this->mb_question = strdup(str);
    this->mb_mode = true;
    this->mb_response_index = 0;
    this->mb_response_func = func;
    this->mb_response_usr = usr;

    mb_ask_rewrite(this);

    return;
}

static void update_sb(screen_internal_t *this)
{
    DISPLAY_SIZE(w, h);
    uint sb_y = h-2;

    uint x, y;
    this->display->get_cursor(this->display, &x, &y);
    this->display->set_cursor(this->display, 0, sb_y);

    varray_t *s = varray_create();
    display_style_t *ds = display_style_create_default();
    ds->bg_color = DISPLAY_COLOR_WHITE;
    ds->bg_bright = false;
    ds->fg_color = DISPLAY_COLOR_BLACK;
    ds->fg_bright = true;
    ds->highlight = false;
    varray_add(s, ds);
    this->display->set_styles(this->display, s);

    this->display->write(this->display, NULL, w, 0);

    this->display->remove_styles(this->display);
    display_style_destroy(ds);
    varray_destroy(s);

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
    DISPLAY_SIZE(w, h);
    ASSERT(lineno + FOOTER_SIZE < h, "tried to write to an invalid line");

    this->display->set_cursor(this->display, 0, lineno);
    this->display->set_styles(this->display, line->styles);

    uint index = 0;
    uint line_len = strlen(line->data);
    uint num_styles = varray_size(line->styles);
    buffer_line_region_t *r;

    varray_iter(r, line->regions) {
        ASSERT(r != NULL, "line->regions contains NULL pointers");

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
    uint x, y;
    this->display->get_cursor(this->display, &x, &y);
    this->display->write(this->display, NULL, w-(x+1), -1);
}

static void update_cursor(screen_internal_t *this)
{
    DEBUG("inside screen->update_cursor(): entering\n");

    uint x, y;
    this->cb->get_cursor(this->cb, &x, &y);
    DEBUG("x=%d, y=%d\n", x, y);

    uint w, h;
    this->display->get_size(this->display, &w, &h);

    // first: check the viewport
    uint *vpy = &this->cb_viewport_y;
    DEBUG("*vpy=%d\n", *vpy);
    bool adjust_viewport = false;
    bool adjust_go_up = false;

    if(y < *vpy) {
        adjust_viewport = true;
        adjust_go_up = true;
    }

    else {
        uint vpy_end = *vpy + h - FOOTER_SIZE;
        if(y >= vpy_end) {
           adjust_viewport = true;
           adjust_go_up = false;
        }
    }

    DEBUG("adjust_viewport=%d, adjust_go_up=%d\n",
          adjust_viewport, adjust_go_up);

    // second: adjust viewport if needed (and force a full redraw)
    if(adjust_viewport) {
        if(adjust_go_up)
            *vpy = (h/2 <= *vpy) ? (*vpy - h/2) : 0;
        else
            *vpy += h/2;

        DEBUG("new *vpy=%d\n", *vpy);
        DEBUG("new this->cb_viewport_y=%d\n", this->cb_viewport_y);
        update_all(this);
    }

    else {
        update_sb(this); // always update the status bar
        this->display->set_cursor(this->display, x, y - *vpy);
    }

    DEBUG("inside screen->update_cursor(): leaving\n");
}

static void update_all(screen_internal_t *this)
{
    DEBUG("inside screen->update_all(): entering\n");

    uint w, h;
    this->display->get_size(this->display, &w, &h);
    h -= FOOTER_SIZE;

    uint *vpy = &this->cb_viewport_y;

    // write the lines in the buffer_t
    uint numlines = this->cb->num_lines(this->cb);
    int i;
    for(i = 0; i + *vpy < numlines && i < h; i++) {
        buffer_line_t *line = this->cb->get_line_data_fmt(this->cb, i + *vpy);
        display_write_line(this, line, i);
        buffer_line_destroy(line);
    }

    // clear the remainer of the screen
    for(; i < h; i++) {
        this->display->write(this->display, NULL, w, -1);
    }

    update_sb(this); // always update the status bar

    DEBUG("inside screen->update_all: updating cursor()\n");
    update_cursor(this);

    DEBUG("inside screen->update_all: leaving\n");
}

static void refresh(screen_t *scrn)
{
    update_all(cast_this(scrn));
}

static void finish_mb_ask(screen_internal_t *this, bool complete)
{
    // add a NULL to the end of mb_response
    this->mb_response[this->mb_response_index] = '\0';

    // fix the screen...
    mb_write(this->super, NULL);
    update_cursor(this);

    // send the data off to the original requester
    this->mb_mode = false;
    this->mb_response_index = 0;
    if(this->mb_question != NULL) {
        free(this->mb_question);
        this->mb_question = NULL;
    }

    char *data = complete ? this->mb_response : NULL;
    if(this->mb_response_func != NULL)
        this->mb_response_func(this->mb_response_usr, data);

    this->mb_response[0] = '\0';
    this->mb_response_func = NULL;
    this->mb_response_usr = NULL;
}

static void mb_ask_rewrite(screen_internal_t *this)
{
    DISPLAY_SIZE(w, h);

    const int BUFSZ = 256;
    char buffer[BUFSZ+1];
    uint n = snprintf(buffer, BUFSZ,
                      "%s: %s", this->mb_question, this->mb_response);
    mb_write(this->super, buffer);
    this->display->set_cursor(this->display, n, h-1);
}

static bool mb_key_handler(screen_internal_t *this, key_event_t *e)
{
    ASSERT(this->mb_mode, "mb_key_handler() called, but not in mb_mode");

    u32 c = e->key_code;
    char ch = (char)c;

    // echo any visible keys
    if(is_visible(c)) {
        if(this->mb_response_index >= MB_RESPONSE_SIZE) {
            ERROR("Overflow on mb_response... Ignoring...\n");
        } else {
            this->display->write(this->display, &ch, 1, -1);
            this->mb_response[this->mb_response_index++] = ch;
        }
    } else {
        // check for Enter key
        switch(c) {

            case KBRD_CTRL('g'):
                finish_mb_ask(this, false);
                break;

            case '\n':
            case KBRD_ENTER:
                finish_mb_ask(this, true);

            case KBRD_BACKSPACE:
                if(this->mb_response_index > 0) {
                    this->mb_response[--this->mb_response_index] = '\0';
                    mb_ask_rewrite(this);
                }

            default:
                ERROR("Invisible key on mb_response... Ignoring...\n");
        }
    }

    return true;
}

static void open_file(screen_internal_t *this, const char *filename)
{
    if(filename == NULL) {
        DEBUG("open_file() received filename=NULL, ignoring...\n");
        return;
    }

    const int BUFSZ = 256;
    char buffer[BUFSZ+1];

    screen_t *s = this->super;
    buffer_t *b = fileio_load_buffer(filename);
    if(b == NULL) {
        snprintf(buffer, BUFSZ, "failed to read: %s", filename);
        mb_write(s, buffer);
    } else {
        s->set_active_buffer(s, s->add_buffer(s, b));
        update_all(this);
    }

}

static bool key_handler(void *usr, key_event_t *e)
{
    screen_internal_t *this = (screen_internal_t *)usr;
    bool ret = false;

    const int BUFSZ = 256;
    char buffer[BUFSZ+1];

    DEBUG("inside key_handler(): entering\n");

    if(this->mb_mode) {
        DEBUG("key will be handled by message box mode\n");
        return mb_key_handler(this, e);
    }

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
        mb_write(this->super, buffer);
    }

    else if(c == KBRD_CTRL('l')) {
        sprintf(buffer, "|x=%d y=%d|", x, y);
        mb_write(this->super, buffer);
    }

    else if(c == KBRD_CTRL('g')) {
        mb_write(this->super, NULL);
    }

    else if(c == KBRD_CTRL('f')) {
        mb_ask(this->super, "File", (mb_response_func_t)open_file, this);
    }

    else if(c == KBRD_CTRL('w')) {
        const char *filename = this->cb->get_filename(this->cb);
        bool success = fileio_save_buffer(this->cb, filename);
        if(!success) {
            snprintf(buffer, BUFSZ, "failed to save: %s", filename);
        } else {
            snprintf(buffer, BUFSZ, "successfully saved: %s", filename);
        }
        mb_write(this->super, buffer);
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

    this->mb_mode = false;
    this->mb_response_func = NULL;
    this->mb_response_usr = NULL;
    this->mb_question = NULL;

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
    s->mb_write = mb_write;
    s->mb_ask = mb_ask;
    s->destroy = destroy;
    s->refresh = refresh;

    return s;
}
