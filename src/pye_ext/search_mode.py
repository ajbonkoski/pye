from pye import *
from pye_ext import global_settings as GS
from pye_ext import common_func as CF

class SearchMode:

    def begin_mode(self, b):
        debug("SearchMode: begin_mode");
        screen.trigger_mode("mb_ask", "Search", self.handle_mb_ask_result)
        return True

    def handle_mb_ask_result(self, data):
        debug("mb ask is complete - result='{}'".format(data))
        b = screen.get_active_buffer()
        b.enable_highlight(2, 4, 7, 7, 2)
        debug("handle_mb_ask_result returning...")
        return None

    def on_key(self, key):
        debug("SearchMode: on_key");
        return (True, False)
