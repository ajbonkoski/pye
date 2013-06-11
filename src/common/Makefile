include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

COMMON_OBJS = common.o string_lib.o timeutil.o varray.o gap_buffer.o

LIBCOMMON=$(LIB_PATH)/libcommon.a

all: $(LIBCOMMON)

$(LIBCOMMON): $(COMMON_OBJS)
	ar rc $@ $(COMMON_OBJS)

clean:
	rm -f *.o $(LIBCOMMON)
