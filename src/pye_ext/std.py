from pye import *
from pye_ext import syntax_highlighter

def init():
    screen.on_buffer_added(buf_handler)

def buf_handler(index):
    debug("Buffer #{} added".format(index))
    b = screen.get_buffer(index)
    fn = b.get_filename()
    if fn != None and '.' in fn:
        ext = fn.split('.')[-1]
        debug("buf_handler: filename='{}', ext='{}'".format(fn, ext));
        func = syntax_highlighter.create(ext)
        if func != None: b.register_formatter(func)
        debug("Formatter registered on buffer #{}".format(index))
