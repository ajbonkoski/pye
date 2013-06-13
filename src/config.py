from pye import *

debug("inside python config")

x, y = display.get_cursor()
w, h = display.get_size()
debug("loc: ({}, {})".format(x, y))
debug("size: ({}, {})".format(w, h))
#display.write("Written from Python!")
screen.write_mb("Wrote from Python!")
debug("after")
