from pye import *
from pye_ext import global_settings as GS
from pye_ext import common_func as CF

class SearchMode:

    def begin_mode(self, b):
        b.enable_highlight(2, 4, 7, 7, 2)
        debug("SearchMode: begin_mode");

    def on_key(self, key):
        debug("SearchMode: on_key");
        return (True, False)
