include ../common.mk

CFLAGS = $(CFLAGS_STD) $(CFLAGS_COMMON) $(CFLAGS_EXEC)
LDFLAGS = $(LDFLAGS_STD) $(LDFLAGS_COMMON) $(LDFLAGS_EXEC)
LIB_DEP = $(LIB_COMMON) $(LIB_DISPLAY) $(LIB_SCREEN) $(LIB_BUFFER) $(LIBFILEIO) $(LIBEXEC)

EXEC_OBJS = execution.o execution_display.o execution_keyboard.o execution_color.o execution_screen.o execution_buffer.o execution_kill_buffer.o execution_varargs.o
LIBEXEC=$(LIB_PATH)/libexecution.a

ALL = $(LIBEXEC)
all: $(ALL)

$(LIBEXEC): $(EXEC_OBJS) $(LIB_DEP)
	ar rc $@ $^

clean:
	rm -f *.o $(ALL)
