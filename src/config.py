\from pye import *
from pye_ext import std
from pye_ext import emacs_mode
from pye_ext import global_settings as GS

## standard functionality init
std.init()
emacs_mode.init()

## User special config
GS.TAB_ENABLE = False
GS.TAB_SIZE = 4
