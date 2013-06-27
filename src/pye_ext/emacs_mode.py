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

def handle_buffer_key(b, key):

    debug('got key={}'.format(key))

    if key == ord('\t'):
        b.insert(GS.get_tab())
        return True

    if key == keyboard.CTRL_SPACE:
        CF.set_mark(b)
        return True

    if key == keyboard.CTRL('v'):
        CF.scroll_down(b)
        return True

    if key == keyboard.CTRL('b'):
        CF.scroll_up(b)
        return True

    if key == keyboard.CTRL('k'):
        CF.kill_line(b)
        return True

    if key == keyboard.CTRL('n'):
        CF.copy_line(b)
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
        CF.yank(b)
        return True

    if key == keyboard.CTRL('w'):
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

    if key == keyboard.CTRL('q'):
        if not b.has_mark():
            screen.mb_write("no mark")
        else:
            sx, sy = b.get_mark()
            ex, ey = b.get_cursor()
            data = b.get_region_data(sx, sy, ex, ey)
            debug("Ctrl-t Region: '{}'".format(data))
            screen.mb_write("grabbed region: ({}, {}) -> ({}, {})".format(sx, sy, ex, ey))
            killbuffer.add(data)
            b.clear_mark()
            b.remove_region_data(sx, sy, ex, ey)
            b.set_cursor(sx, sy)
        return True

    if key == keyboard.CTRL('g'):
        b.clear_mark()
        screen.refresh()
        return False  ## we return false here, so the standard clear functions can run

    return False
