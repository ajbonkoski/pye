from pye import *
#from pye_ext import syntax_highlighter

def init():
    debug("inside python config")

    ## this is a hack, the formatter should really be added
    ## on buffer creation (triggered by a screen event)
    b = screen.get_active_buffer()
    #b.register_formatter(syntax_highlighter.fmt_handler)
    debug("formatter registered")

    debug("keyboard.ARROW_LEFT={}".format(keyboard.ARROW_LEFT))
    debug("Ctrl-u = {}".format(keyboard.CTRL('u')))

    x, y = display.get_cursor()
    debug("loc: ({}, {})".format(x, y))

    w, h = display.get_size()
    debug("size: ({}, {})".format(w, h))

    #display.write("Written from Python!")
    screen.write_mb("Wrote from Python!")

    ## register key handler
    screen.onkey(key_handler)

def key_handler(key):
    if key == keyboard.CTRL('t'):
        screen.write_mb("python message")
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
