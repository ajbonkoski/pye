from pye import *

def init():
    debug("inside python config")

    x, y = display.get_cursor()
    debug("loc: ({}, {})".format(x, y))

    w, h = display.get_size()
    debug("size: ({}, {})".format(w, h))

    #display.write("Written from Python!")
    screen.write_mb("Wrote from Python!")

    ## register key handler
    screen.onkey(key_handler)

def key_handler(key):
    debug("python received key event ({})".format(key))

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

    return False


init()
