#include "screen.h"

#define IMPL_TYPE 0x7c870c4b
#define BLANK ' '

typedef struct
{
    screen_t *super;

    display_t *display;
    uint mb_y;
    uint disp_width;

    varray_t *buffers;
    buffer_t *cb;  /* the current buffer */

} screen_internal_t;
static screen_internal_t *cast_this(screen_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a screen_internal_t object");
    return (screen_internal_t *)s->impl;
}

// forward declarations
static void add_buffer(screen_t *scrn, buffer_t *buffer);
static varray_t *list_buffers(screen_t *scrn);
static void set_buffer(screen_t *scrn, uint id);
static void write_mb(screen_t *scrn, const char *str);
static void destroy(screen_t *scrn);


static void add_buffer(screen_t *scrn, buffer_t *buffer)
{
    screen_internal_t *this = cast_this(scrn);
    varray_add(this->buffers, buffer);
    if(this->cb == NULL)
        this->cb = buffer;
}

static varray_t *list_buffers(screen_t *scrn)
{
    screen_internal_t *this = cast_this(scrn);
    return this->buffers;
}

static void set_buffer(screen_t *scrn, uint id)
{
    screen_internal_t *this = cast_this(scrn);
    ASSERT(id >= 0 && id < varray_size(this->buffers), "Index out-of-bounds error");
    this->cb = varray_get(this->buffers, id);
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
    this->display->write(this->display, buf, this->disp_width);
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

static bool key_handler(void *usr, key_event_t *e)
{
    screen_internal_t *this = (screen_internal_t *)usr;

    u32 c = e->key_code;
    //char ch = (char)c;
    char buffer[256];

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

    // "normal" keystrokes - reroute these to the buffer
    else {
        if(this->cb != NULL)
            this->cb->input_key(this->cb, c);
    }

    /* else if(c == KBRD_ARROW_LEFT) { */
    /*     if(x > 0) */
    /*         this->display->set_cursor(this->display, x-1, y); */
    /* } */

    /* else if(c == KBRD_ARROW_RIGHT) { */
    /*     if(x < w-1) */
    /*         this->display->set_cursor(this->display, x+1, y); */

    /* } */

    /* else if(c == KBRD_ARROW_UP) { */
    /*     if(y > 0) */
    /*         this->display->set_cursor(this->display, x, y-1); */

    /* } */

    /* else if(c == KBRD_ARROW_DOWN) { */
    /*     if(y < h-2) */
    /*         this->display->set_cursor(this->display, x, y+1); */

    /* } */

    /* else { */
    /*     this->display->write(this->display, &ch, 1); */
    /* } */

    //DEBUG("%d:%d:%d:%d\n", c, KEY_CTRL('s'), KEY_CTRL('l'), KEY_CTRL('g'));

    return true;
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
    return this;
}

screen_t *screen_create(display_t *disp)
{
    screen_t *s = calloc(1, sizeof(screen_t));
    s->impl = screen_create_internal(s, disp);
    s->impl_type = IMPL_TYPE;

    s->add_buffer = add_buffer;
    s->list_buffers = list_buffers;
    s->set_buffer = set_buffer;
    s->write_mb = write_mb;
    s->destroy = destroy;

    return s;
}
