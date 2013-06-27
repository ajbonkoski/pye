from pye import *
from pye_ext import global_settings as GS
from pye_ext import common_func as CF

def init():
    screen.on_key(key_handler)

def key_handler(key):

    b = screen.get_active_buffer()
    if handle_buffer_key(b, key):
        screen.refresh()
        return True

    return False


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
    keyboard.CTRL('w'):   CF.kill_region,
    keyboard.CTRL('q'):   CF.copy_region,
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
