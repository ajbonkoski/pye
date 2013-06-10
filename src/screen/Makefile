include ../common.mk

# display_t CFLAGS needed, but we don't need to link (just need the typedef)
CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_DISPLAY)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

SCREEN_OBJS = screen.o
LIBSCREEN=$(LIB_PATH)/libscreen.a

all: $(LIBSCREEN)

$(LIBSCREEN): $(SCREEN_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(LIBSCREEN)
