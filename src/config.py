from pye import *
from pye_ext import syntax_highlighter

def init():
    debug("inside python config")
    debug("keyboard.ARROW_LEFT={}".format(keyboard.ARROW_LEFT))

    x, y = display.get_cursor()
    debug("loc: ({}, {})".format(x, y))

    w, h = display.get_size()
    debug("size: ({}, {})".format(w, h))

    #display.write("Written from Python!")
    screen.write_mb("Wrote from Python!")

    ## register key handler
    screen.onkey(key_handler)

def key_handler(key):

    if key == 20:
        screen.write_mb("python message")
        return True

    if key == 25:
        b = screen.get_active_buffer()
        debug("Got the active buffer");
        b.insert('A');
        debug("Added character to active buffer");
        screen.refresh();
        return True

    if key == 11:
        handle_kill()
        screen.refresh();
        return True

    return False

def handle_kill():
    b = screen.get_active_buffer()
    x, y = b.get_cursor()
    ll = b.line_len(y)
    DEL_KEY = 19<<8

    num_to_del = ll-x
    if num_to_del == 0:
        num_to_del = 1 # if blank line, delete newline char

    for i in range(num_to_del):
        debug("inserting key i={}".format(i))
        b.insert(DEL_KEY) # insert a DEL key

    debug("handle_kill done")

init()
