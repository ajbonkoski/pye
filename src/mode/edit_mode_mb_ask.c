#include "edit_mode_mb_ask.h"
#include "display/keyboard.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define IMPL_TYPE 0x28004c5
#define MB_RESPONSE_SIZE 200
typedef void (*mb_response_func_t)(void *usr, char *response);

typedef struct {

    edit_mode_t *super;

    screen_t *scrn;
    bool mode_running;
    mb_response_func_t mb_response_func;
    void *mb_response_usr;
    char *mb_question;
    char mb_response[MB_RESPONSE_SIZE];
    uint mb_response_index;

} edit_mode_mb_ask_t;

// inline functions
static inline edit_mode_mb_ask_t *cast_this(edit_mode_t *s)
{
    ASSERT(s->impl_type == IMPL_TYPE, "expected a edit_mode_mb_ask_t object");
    return (edit_mode_mb_ask_t *)s->impl;
}

// forward declarations
static void mb_redraw(edit_mode_mb_ask_t *this);
static void begin_mode(edit_mode_t *m, va_list args);
static bool on_key(edit_mode_t *m, key_event_t *key);
static void destroy(edit_mode_t *m);

static void mb_redraw(edit_mode_mb_ask_t *this)
{
    uint mb_y = this->scrn->mb_get_yloc(this->scrn);

    const int BUFSZ = 256;
    char buffer[BUFSZ+1];
    uint n = snprintf(buffer, BUFSZ,
                      "%s: %s", this->mb_question, this->mb_response);
    this->scrn->mb_write(this->scrn, buffer);
    this->scrn->set_cursor(this->scrn, n, mb_y);
}

static void finish_mb_ask(edit_mode_mb_ask_t *this, bool complete)
{
    // add a NULL to the end of mb_response
    this->mb_response[this->mb_response_index] = '\0';

    // fix the screen...
    this->scrn->mb_write(this->scrn, NULL);

    //update_cursor(this); // todo: is this needed?

    // send the data off to the original requester
    this->mode_running = false;
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


static void begin_mode(edit_mode_t *m, va_list args)
{
    DEBUG("inside begin_mode()\n");
    edit_mode_mb_ask_t *this = cast_this(m);

    // get the question
    const char *question = va_arg(args, const char *);
    ASSERT(question != NULL, "edit_mode_mb_ask_t->begin_mode() was passed question == NULL!");
    if(this->mb_question != NULL)
        free(this->mb_question);
    this->mb_question = strdup(question);

    // get the response func
    this->mb_response_func = va_arg(args, mb_response_func_t);
    ASSERT(this->mb_response_func != NULL, "edit_mode_mb_ask_t->begin_mode() was passed mb_response_func == NULL!");

    // get the response usr data
    this->mb_response_usr = va_arg(args, void *);
    // Note: its perfectly acceptable if usr is NULL here

    // enable the mode
    this->mode_running = true;
    mb_redraw(this);
}

static bool on_key(edit_mode_t *m, key_event_t *e)
{
     edit_mode_mb_ask_t *this = cast_this(m);
    /* ASSERT_UNIMPL(); */
    /* return false; */


    ASSERT(this->mode_running, "edit_mode_mb_ask->on_key() called, but not in mb_mode");

    u32 c = e->key_code;
    char ch = (char)c;

    // echo any visible keys
    if(key_is_visible(c)) {
        if(this->mb_response_index >= MB_RESPONSE_SIZE) {
            ERROR("Overflow on mb_response... Ignoring...\n");
        } else {
            //this->display->write(this->display, &ch, 1, -1);
            this->mb_response[this->mb_response_index++] = ch;
            mb_redraw(this);
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
                    mb_redraw(this);
                }

            default:
                ERROR("Invisible key on mb_response... Ignoring...\n");
        }
    }

    return true;

}

static void destroy(edit_mode_t *m)
{
    edit_mode_mb_ask_t *this = cast_this(m);
    if(this->mb_question != NULL)
        free(this->mb_question);

    free(this);
    free(m);
}


static edit_mode_mb_ask_t *edit_mode_create_internal(edit_mode_t *m, screen_t *scrn)
{
    edit_mode_mb_ask_t *this = calloc(1, sizeof(edit_mode_mb_ask_t));
    this->super = m;
    this->scrn = scrn;
    this->mode_running = false;
    this->mb_response_func = NULL;
    this->mb_response_usr = NULL;
    this->mb_question = NULL;
    this->mb_response_index = 0;

    return this;
}

edit_mode_t *edit_mode_mb_ask_create(screen_t *screen)
{
    edit_mode_t *m = calloc(1, sizeof(mode_t));
    m->impl = edit_mode_create_internal(m, screen);
    m->impl_type = IMPL_TYPE;

    m->begin_mode = begin_mode;
    m->on_key = on_key;
    m->destroy = destroy;

    return m;
}
