from pye import *

def init():
    screen.on_key(key_handler)

def key_handler(key):

    if key == keyboard.CTRL('k'):
        handle_kill()
        screen.refresh();
        return True

    if key == keyboard.CTRL('a'):
        b = screen.get_active_buffer()
        b.goto_line_start()
        screen.refresh()
        return True

    if key == keyboard.CTRL('e'):
        b = screen.get_active_buffer()
        b.goto_line_end()
        screen.refresh()
        return True

    if key == keyboard.CTRL('d'):
        b = screen.get_active_buffer()
        b.insert(keyboard.DEL)
        screen.refresh()
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

