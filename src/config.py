from pye import *
from pye_ext import std
from pye_ext import emacs_mode
from pye_ext import settings

## standard functionality init
std.init()
emacs_mode.init()

## User special config
settings.TAB_ENABLE = False
settings.TAB_SIZE = 4
