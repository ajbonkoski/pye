include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_BUFFER)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON) $(LDFLAGS_BUFFER)

MODE_OBJS = edit_mode_mb_ask.o
LIBMODE=$(LIB_PATH)/libmode.a

ALL = $(LIBMODE)
all: $(ALL)

$(LIBMODE): $(MODE_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(ALL)
