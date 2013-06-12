include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_BUFFER)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON) $(LDFLAGS_BUFFER)

FILEIO_OBJS = fileio.o
LIBFILEIO=$(LIB_PATH)/libfileio.a

ALL = $(LIBFILEIO)
all: $(ALL)

$(LIBFILEIO): $(FILEIO_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(ALL)
