include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

BUFFER_OBJS = buffer.o data_buffer.o
LIBBUFFER=$(LIB_PATH)/libbuffer.a

all: $(LIBBUFFER)

$(LIBBUFFER): $(BUFFER_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(LIBBUFFER)
