include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON)

COMMON_OBJS = common.o string_lib.o timeutil.o varray.o gap_buffer.o

TEST_OBJS = test/gap_buffer_test.o
TEST_BIN =$(BIN_PATH)/test/gap_buffer

LIBCOMMON=$(LIB_PATH)/libcommon.a

all: $(LIBCOMMON) $(TEST_BIN)

$(LIBCOMMON): $(COMMON_OBJS)
	ar rc $@ $(COMMON_OBJS)

$(TEST_BIN): $(TEST_OBJS) $(LIBCOMMON)
	$(CC) -o $@ $^

clean:
	rm -f *.o test/*.o $(ALL)
