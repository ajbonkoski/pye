include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

DISPLAY_OBJS = display.o display_terminal.o termio.o

LIBDISPLAY=$(LIB_PATH)/libdisplay.a

all: $(LIBDISPLAY)

$(LIBDISPLAY): $(DISPLAY_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(LIBDISPLAY)
