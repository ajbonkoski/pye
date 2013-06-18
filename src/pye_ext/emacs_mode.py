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

    if key == keyboard.CTRL('t'):
        x, y = b.get_cursor()
        b.set_mark(x, y)
        screen.mb_write("mark: ({}, {})".format(x, y))
        return True

    if key == keyboard.CTRL('p'):
        if b.has_mark():
            x, y = b.get_mark()
            screen.mb_write("mark: ({}, {})".format(x, y))
        else:
            screen.mb_write("no mark")

        return True

    if key == keyboard.CTRL('r'):
        screen.mb_write("C-r")
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

