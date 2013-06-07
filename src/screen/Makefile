include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

SCREEN_OBJS = screen.o screen_terminal.o termio.o

LIBSCREEN=$(LIB_PATH)/libscreen.a

all: $(LIBSCREEN)

$(LIBSCREEN): $(SCREEN_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(LIBSCREEN)
