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

    if key == ord('\t'):
        b.insert(' '*4)
        return True

    if key == keyboard.CTRL('v'):
        x, y = b.get_cursor()
        w, h = display.get_size()
        vpy = screen.get_viewport_line()
        nlines = b.num_lines()
        y = vpy + h + h/2
        if y > nlines-1:
            y = nlines - 1
        b.set_cursor(x, y)
        #screen.mb_write("Ctrl-v")

    if key == keyboard.CTRL('b'):
        x, y = b.get_cursor()
        w, h = display.get_size()
        vpy = screen.get_viewport_line()
        nlines = b.num_lines()
        y = vpy - h/2
        if y < 0:
            y = 0
        b.set_cursor(x, y)
        #screen.mb_write("Ctrl-v")

    if key == keyboard.CTRL('k'):
        handle_kill(b)
        return True

    if key == keyboard.CTRL('a'):
        b.goto_line_start()
        return True

    if key == keyboard.CTRL('e'):
        b.goto_line_end()
        return True

    if key == keyboard.CTRL('d'):
        b.insert_key(keyboard.DEL)
        return True

    if key == keyboard.CTRL('y'):
        if killbuffer.get_size() == 0:
            screen.mb_write("Kill-buffer is empty")
        else:
            data = killbuffer.get(0)
            debug("Yank Data: '{}'".format(data))
            b.insert(data)
        return True

    if key == keyboard.CTRL('r'):
        x, y = b.get_cursor()
        b.set_mark(x, y)
        screen.mb_write("mark: ({}, {})".format(x, y))
        debug("kill_buffer size={}, max={}".format(killbuffer.get_size(), killbuffer.get_max_size()))
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
            killbuffer.add(data)
        return True

    return False


def handle_kill(b):
    x, y = b.get_cursor()
    ll = b.line_len(y)

    num_to_del = ll-x
    if num_to_del == 0:
        num_to_del = 1
        data = b.get_region_data(x, y, ll, y)

    else:
        data = b.get_region_data(x, y, ll-1, y)

    killbuffer.add(data)
    debug("kill data='{}'".format(data))

    for i in range(num_to_del):
        b.insert_key(keyboard.DEL) # insert a DEL key
