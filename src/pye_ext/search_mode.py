from pye import *
from pye_ext import global_settings as GS
from pye_ext import common_func as CF

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
            debug("({}, {}) => ({}, {})".format(sx, sy, ex, ey))

    def on_key(self, key):
        debug("SearchMode: on_key");
        self.enable_region_highlight(False)
        return (True, False)
