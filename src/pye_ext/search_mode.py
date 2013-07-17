from pye import *
from pye_ext import global_settings as GS
from pye_ext import common_func as CF

def point_cmp(x1, y1, x2, y2):
    if y1 != y2: return y1 - y2
    else:        return x1 - x2

class SearchMode:

    def begin_mode(self, b):
        screen.trigger_mode("mb_ask", "Search", self.handle_mb_ask_result)
        return True

    def handle_mb_ask_result(self, data):
        b = screen.get_active_buffer()
        self.search_buffer_for_regions(b, data)
        self.enable_region_highlight(True)

    def search_buffer_for_regions(self, b, data):
        self.regions = []
        datalen = len(data)
        for n in xrange(b.num_lines()):
            line = b.get_line_data(n)
            loc = line.find(data)
            if loc != -1:
                self.regions.append((loc, n, loc+datalen, n))

    def enable_region_highlight(self, enable):
        b = screen.get_active_buffer()
        for sx, sy, ex, ey in self.regions:
            b.enable_highlight(sx, sy, ex, ey, 1 if enable else 0)

    def next_match(self):
        b = screen.get_active_buffer()
        x, y = b.get_cursor()
        found = False
        for sx, sy, ex, ey in self.regions:
            if point_cmp(sx, sy, x, y) > 0:
                found = True
                break

        if found:
            b.set_cursor(sx, sy)
            screen.center_viewport()
            screen.refresh()

    def on_key(self, key):
        debug("SearchMode: on_key");
        try:
            if key == keyboard.CTRL('g'):
                self.enable_region_highlight(False)
                return (True, True)
            elif key == keyboard.CTRL('x'):
                self.next_match()
                return (False, True)
            return (False, False)
        except Exception as e:
            import sys
            import traceback
            exc_type, exc_value, exc_traceback = sys.exc_info()
            lines = traceback.format_exception(exc_type, exc_value, exc_traceback)
            debug(''.join(line for line in lines))
            return (True, True)

