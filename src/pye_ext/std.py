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
        b.register_formatter(syntax_highlighter.create(ext))
        debug("Formatter registered on buffer #{}".format(index))


class EditModeDeriveFrom(object):

    def __init__(self, parent):
        self.parent = parent

    def __call__(self, f):
        def wrapped_f(*args):
            f(*args)
        return wrapped_f
