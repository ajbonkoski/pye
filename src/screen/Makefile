include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_DISPLAY)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON) $(CFLAGS_DISPLAY)

SCREEN_OBJS = screen.o
LIBSCREEN=$(LIB_PATH)/libscreen.a

all: $(LIBSCREEN)

$(LIBSCREEN): $(SCREEN_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(LIBSCREEN)
