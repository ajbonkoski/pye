from pye import *
from pye_ext import syntax_highlighter

def init():
    debug("inside python config")
    screen.mb_write(str(color.from_string("red")))

    ## register handlers
    screen.on_key(key_handler)
    screen.on_buffer_added(buf_handler)

def key_handler(key):
    if key == keyboard.CTRL('t'):
        screen.mb_write("python message")
        return True

    if key == keyboard.CTRL('y'):
        b = screen.get_active_buffer()
        debug("Got the active buffer");
        b.insert(ord('A'));
        debug("Added character to active buffer");
        screen.refresh();
        return True

    if key == keyboard.CTRL('k'):
        handle_kill()
        screen.refresh();
        return True

    return False

def buf_handler(index):
    debug("Buffer #{} added".format(index))
    b = screen.get_buffer(index)
    b.register_formatter(syntax_highlighter.create_fmt_handler("py"))
    debug("Formatter registered on buffer #{}".format(index))

def handle_kill():
    b = screen.get_active_buffer()
    x, y = b.get_cursor()
    ll = b.line_len(y)

    num_to_del = ll-x
    if num_to_del == 0:
        num_to_del = 1 # if blank line, delete newline char

    for i in range(num_to_del):
        b.insert(keyboard.DEL) # insert a DEL key

init()
