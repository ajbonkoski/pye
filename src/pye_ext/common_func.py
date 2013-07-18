from pye import *
from pye_ext import global_settings as GS

def kill_line(b):
    x, y = b.get_cursor()
    ll = b.line_len(y)

    if ll-x == 0:
        sx, sy, ex, ey = x, y, 0, y+1

    else:
        sx, sy, ex, ey = x, y, ll, y

    data = b.get_region_data(sx, sy, ex, ey)
    killbuffer.add(data)
    debug("kill data='{}'".format(data))
    b.remove_region_data(sx, sy, ex, ey)

def copy_line(b):
    x, y = b.get_cursor()
    ll = b.line_len(y)

    if ll-x == 0:
        sx, sy, ex, ey = x, y, 0, y+1

    else:
        sx, sy, ex, ey = x, y, ll, y

    data = b.get_region_data(sx, sy, ex, ey)
    killbuffer.add(data)
    debug("copy data='{}'".format(data))

def set_mark(b):
    x, y = b.get_cursor()
    b.set_mark(x, y)
    screen.mb_write("mark: ({}, {})".format(x, y))
    debug("kill_buffer size={}, max={}".format(killbuffer.get_size(), killbuffer.get_max_size()))

def yank(b):
    if killbuffer.get_size() == 0:
        screen.mb_write("Kill-buffer is empty")
    else:
        data = killbuffer.get(0)
        debug("Yank Data: '{}'".format(data))
        b.insert(data)

def scroll_up(b):
    x, y = b.get_cursor()
    w, h = display.get_size()
    vpy = screen.get_viewport_line()
    nlines = b.num_lines()
    y = vpy - h/2
    if y < 0:
        y = 0
    b.set_cursor(x, y)

def scroll_down(b):
    x, y = b.get_cursor()
    w, h = display.get_size()
    vpy = screen.get_viewport_line()
    nlines = b.num_lines()
    y = vpy + h + h/2
    if y > nlines-1:
        y = nlines - 1
    b.set_cursor(x, y)

def insert_newline(b):
    x, y = b.get_cursor()
    b.insert("\n")
    b.set_cursor(x, y)

def copy_region(b):
    if not b.has_mark():
        screen.mb_write("no mark")
    else:
        sx, sy = b.get_mark()
        ex, ey = b.get_cursor()
        data = b.get_region_data(sx, sy, ex, ey)
        screen.mb_write("grabbed region: ({}, {}) -> ({}, {})".format(sx, sy, ex, ey))
        killbuffer.add(data)
        b.clear_mark()
        return True

def kill_region(b):
    if not b.has_mark():
        screen.mb_write("no mark")
    else:
        sx, sy = b.get_mark()
        ex, ey = b.get_cursor()
        data = b.get_region_data(sx, sy, ex, ey)
        screen.mb_write("grabbed region: ({}, {}) -> ({}, {})".format(sx, sy, ex, ey))
        killbuffer.add(data)
        b.clear_mark()
        b.remove_region_data(sx, sy, ex, ey)
        b.set_cursor(sx, sy)


## basically: API-level aliases
def goto_line_start(b): b.goto_line_start()
def goto_line_end(b): b.goto_line_end()
