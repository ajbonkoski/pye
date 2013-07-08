include ../common.mk

# display_t CFLAGS needed, but we don't need to link (just need the typedef)
CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_MODE)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON) $(LDFLAGS_MODE)

SCREEN_OBJS = screen.o kill_buffer.o
LIBSCREEN=$(LIB_PATH)/libscreen.a

all: $(LIBSCREEN)

$(LIBSCREEN): $(SCREEN_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(LIBSCREEN)
