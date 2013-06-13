include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_EXEC)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON) $(LDFLAGS_EXEC)

EXEC_OBJS = execution.o execution_display.o execution_screen.o
LIBEXEC=$(LIB_PATH)/libexecution.a

ALL = $(LIBEXEC)
all: $(ALL)

$(LIBEXEC): $(EXEC_OBJS)
	ar rc $@ $^

clean:
	rm -f *.o $(ALL)
