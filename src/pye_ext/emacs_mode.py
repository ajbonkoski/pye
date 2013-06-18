from pye import *

def init():
    screen.on_key(key_handler)

def key_handler(key):

    b = screen.get_active_buffer()
    if handle_buffer_key(b, key):
        screen.refresh();
        return True

    return False

def handle_buffer_key(b, key):

    if key == keyboard.CTRL('k'):
        handle_kill()
        return True

    if key == keyboard.CTRL('a'):
        b.goto_line_start()
        return True

    if key == keyboard.CTRL('e'):
        b.goto_line_end()
        return True

    if key == keyboard.CTRL('d'):
        b.insert(keyboard.DEL)
        return True

    if key == keyboard.CTRL('r'):
        x, y = b.get_cursor()
        b.set_mark(x, y)
        screen.mb_write("mark: ({}, {})".format(x, y))
        return True

    if key == keyboard.CTRL('t'):
        if not b.has_mark():
            screen.mb_write("no mark")
        else:
            sx, sy = b.get_mark()
            ex, ey = b.get_cursor()
            data = b.get_region_data(sx, sy, ex, ey)
            debug("Ctrl-t Region: '{}'".format(data))
            screen.mb_write("grabbed region: ({}, {}) -> ({}, {})".format(sx, sy, ex, ey))
        return True

    return False


def handle_kill():
    x, y = b.get_cursor()
    ll = b.line_len(y)

    num_to_del = ll-x
    if num_to_del == 0:
        num_to_del = 1 # if blank line, delete newline char

    for i in range(num_to_del):
        b.insert(keyboard.DEL) # insert a DEL key

