#include "edit_mode_mb_ask.h"
#include "common/callable.h"
#include "display/keyboard.h"

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

#define IMPL_TYPE 0x28004c5
#define MB_RESPONSE_SIZE 200

typedef struct {

    edit_mode_t *super;

    screen_t *scrn;
    bool mode_running;
    autocomplete_f autocomplete_func;
    void *autocomplete_usr;
    callable_t *mb_response_func;
    char *mb_question;
    char mb_response[MB_RESPONSE_SIZE+1];
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
static void begin_mode(edit_mode_t *m, varargs_t *va);
static edit_mode_result_t on_key(edit_mode_t *m, key_event_t *key);
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

    // send the data off to the original requester
    this->mode_running = false;
    this->autocomplete_func = NULL;
    this->autocomplete_usr = NULL;
    this->mb_response_index = 0;
    if(this->mb_question != NULL) {
        free(this->mb_question);
        this->mb_question = NULL;
    }

    char *data = complete ? this->mb_response : NULL;
    if(this->mb_response_func != NULL) {
        varargs_t *va = varargs_create_v(1, "s", data);
        callable_call(this->mb_response_func, va);
        varargs_destroy(va);
        callable_destroy(this->mb_response_func);
    }

    this->mb_response[0] = '\0';
}


static void begin_mode(edit_mode_t *m, varargs_t *va)
{
    DEBUG("inside begin_mode()\n");
    uint size = varargs_size(va);
    ASSERT(size == 2 || size == 4, "wrong number of args passed to edit_mode_mb_ask->begin_mode()");
    edit_mode_mb_ask_t *this = cast_this(m);

    // get the question
    const char *question = varargs_get(va, 0);
    ASSERT(question != NULL, "edit_mode_mb_ask_t->begin_mode() was passed question == NULL!");
    if(this->mb_question != NULL)
        free(this->mb_question);
    this->mb_question = strdup(question);

    // get the response func
    callable_t *ctmp = varargs_get(va, 1);
    ASSERT(ctmp != NULL, "edit_mode_mb_ask_t->begin_mode() was passed mb_response_func == NULL!");
    this->mb_response_func = callable_copy(ctmp);

    // the third/forth args are an optional "autocomplete" bound-function
    if(size == 4) {
        void *func = varargs_get(va, 2);
        void *usr = varargs_get(va, 3);
        this->autocomplete_func = func;
        this->autocomplete_usr = usr;
    } else {
        this->autocomplete_func = NULL;
        this->autocomplete_usr = NULL;
    }

    // enable the mode
    this->mode_running = true;
    mb_redraw(this);
}

static edit_mode_result_t on_key(edit_mode_t *m, key_event_t *e)
{
     edit_mode_mb_ask_t *this = cast_this(m);

    ASSERT(this->mode_running, "edit_mode_mb_ask->on_key() called, but not in mb_mode");

    u32 c = e->key_code;
    char ch = (char)c;
    bool finished = false;

    // echo any visible keys
    if(key_is_visible(c)) {
        if(this->mb_response_index >= MB_RESPONSE_SIZE) {
            ERROR("Overflow on mb_response... Ignoring...\n");
        } else {
            //this->display->write(this->display, &ch, 1, -1);
            this->mb_response[this->mb_response_index++] = ch;
            this->mb_response[this->mb_response_index] = '\0';
            mb_redraw(this);
        }
    } else {
        // check for Enter key
        switch(c) {

            case KBRD_CTRL('g'):
                finish_mb_ask(this, false);
                finished = true;
                break;

            case '\n':
            case KBRD_ENTER:
                finish_mb_ask(this, true);
                finished = true;
                break;

            case KBRD_BACKSPACE:
                if(this->mb_response_index > 0) {
                    this->mb_response[--this->mb_response_index] = '\0';
                    mb_redraw(this);
                }
                break;

            case KBRD_TAB:
                if(this->autocomplete_func != NULL) {
                    char *str = this->autocomplete_func(this->mb_response, this->autocomplete_usr);
                    if(str != NULL) {
                        strncpy(this->mb_response, str, MB_RESPONSE_SIZE);
                        this->mb_response_index = strlen(str);
                        if(this->mb_response_index > MB_RESPONSE_SIZE)
                           this->mb_response_index = MB_RESPONSE_SIZE;
                        this->mb_response[this->mb_response_index] = '\0';
                        free(str);

                        mb_redraw(this);
                    }
                }
                break;

            default:
                ERROR("Invisible key on mb_response... Ignoring...\n");
        }
    }

    edit_mode_result_t res =
        { .finished = finished, .key_handled = true };

    return res;

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
    this->autocomplete_func = NULL;
    this->autocomplete_usr = NULL;
    this->mb_response_func = NULL;
    this->mb_question = NULL;
    this->mb_response_index = 0;

    return this;
}

edit_mode_t *edit_mode_mb_ask_create(screen_t *screen)
{
    edit_mode_t *m = calloc(1, sizeof(edit_mode_t));
    m->impl = edit_mode_create_internal(m, screen);
    m->impl_type = IMPL_TYPE;

    m->begin_mode = begin_mode;
    m->on_key = on_key;
    m->destroy = destroy;

    return m;
}
