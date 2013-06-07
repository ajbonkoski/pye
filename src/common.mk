############################################################
#
# This file should only contain CFLAGS_XXX and LDFLAGS_XXX directives.
# CFLAGS and LDFLAGS themselves should NOT be set: that is the job
# for the actual Makefiles (which will combine the flags given here)
#
# *** DO NOT SET CFLAGS or LDFLAGS  ***
#
# Our recommended flags for all projects. Note -pthread specifies reentrancy

# -Wno-format-zero-length: permit printf("");
# -Wno-unused-parameter: permit a function to ignore an argument

CFLAGS_STD   := -std=gnu99 -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT -Wall -Wno-unused-parameter -Wno-format-zero-length -pthread -fPIC -Werror
LDFLAGS_STD  := -lm

# compute root path by getting the file path for this makefile (ngv/src/common.mk)
# and stripping /src/common.mk from it. this should compute the correct root directory
# no matter what depth common.mk is imported from
ROOT_PATH    := $(subst /src/common.mk,,$(realpath $(lastword $(MAKEFILE_LIST))))
SRC_PATH     := $(ROOT_PATH)/src
BIN_PATH     := $(ROOT_PATH)/bin
LIB_PATH     := $(ROOT_PATH)/lib

CC           := gcc
LD           := gcc

# dynamic libraries
ifeq "$(shell uname -s)" "Darwin"
	LDSH := -dynamic
	SHEXT := .dylib
	WHOLE_ARCHIVE_START := -all_load
else
	LD := gcc
	LDSH := -shared
	SHEXT := .so
	WHOLE_ARCHIVE_START := -Wl,-whole-archive
	WHOLE_ARCHIVE_STOP := -Wl,-no-whole-archive
endif

############################################################
#
# External libraries
#
# List these in roughly the order of dependency; those with fewest
# dependencies first. Within each LDFLAGS, list the dependencies in in
# decreasing order (e.g., end with LDFLAGS_COMMON)
#
############################################################

# common library
CFLAGS_COMMON  := -I$(SRC_PATH)
LDFLAGS_COMMON := $(LIB_PATH)/libcommon.a

# screen library
CFLAGS_SCREEN   :=
LDFLAGS_SCREEEN := $(LIB_PATH)/libscreen.a

%.o: %.c %.h
	@echo "    [$@]"
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	@echo "    [$@]"
	$(CC) $(CFLAGS) -c $<

