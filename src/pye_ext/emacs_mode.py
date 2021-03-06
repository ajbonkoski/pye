from pye import *
from pye_ext import global_settings as GS
from pye_ext import common_func as CF
from pye_ext.search_mode import SearchMode

search_mode = SearchMode()
def init():
    screen.on_key(key_handler)
    debug("adding search mode")
    screen.add_mode("search", search_mode);

def key_handler(key):

    b = screen.get_active_buffer()
    if handle_buffer_key(b, key):
        screen.refresh()
        return True

    return False


def start_search_mode(b):
    debug("starting search mode")
    screen.trigger_mode("search", b)
    return True

def print_buffer_list(b):
    for i,b in enumerate(screen.list_buffers()):
        debug("Buffer id #{}: {}".format(i, b.get_filename()))
    return True


bindings = {
    ord('\t'):            lambda b: b.insert(GS.get_tab()),
    keyboard.CTRL_SPACE:  CF.set_mark,
    keyboard.CTRL('v'):   CF.scroll_down,
    keyboard.CTRL('b'):   CF.scroll_up,
    keyboard.CTRL('k'):   CF.kill_line,
    keyboard.CTRL('o'):   CF.insert_newline,
    keyboard.CTRL('n'):   CF.copy_line,
    keyboard.CTRL('a'):   CF.goto_line_start, #lambda b: b.goto_line_start(),
    keyboard.CTRL('e'):   CF.goto_line_end, #lambda b: b.goto_line_end(),
    keyboard.CTRL('d'):   lambda b: b.insert_key(keyboard.DEL),
    keyboard.CTRL('y'):   CF.yank,
    keyboard.CTRL('w'):   CF.copy_region,
    keyboard.CTRL('q'):   CF.kill_region,
    keyboard.CTRL('s'):   start_search_mode,
    keyboard.CTRL('l'):   print_buffer_list,
    keyboard.CTRL('g'):   CF.switch_buffer,
}

def handle_buffer_key(b, key):

    debug('got key={}'.format(key))

    if key in bindings:
        bindings[key](b)
        return True

    if key == keyboard.CTRL('g'):
        b.clear_mark()
        screen.refresh()
        return False  ## we return false here, so the standard clear functions can run

    return False
