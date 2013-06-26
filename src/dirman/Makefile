include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

DIRMAN_OBJS = dirman.o
LIBDIRMAN=$(LIB_PATH)/libdirman.a

ALL = $(LIBDIRMAN)
all: $(ALL)

$(LIBDIRMAN): $(DIRMAN_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(ALL)
