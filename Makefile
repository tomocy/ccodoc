CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic
LDLIBS := -lm -lncursesw

LIB_SRCS := ccodoc.c renderer.c canvas.c time.c memory.c string.c math.c platform.c
SRCS := main.c mode.c $(LIB_SRCS)
TEST_SRCS := test.c $(LIB_SRCS) ccodoc_test.c renderer_test.c string_test.c time_test.c

_TARGET := $(shell ./tool/build/detect_platform.sh $(TARGET))
ifeq ($(_TARGET),)
$(error failed to detect target platform)
endif

ifeq ($(_TARGET), linux)
CFLAGS += -include ccodoc_macros_linux.h

COMPILE_FLAGS := compile_flags_linux.txt
endif

ifeq ($(_TARGET), macos)
CFLAGS += -include ccodoc_macros_macos.h

VSCODE_SETTINGS := .vscode/settings_macos.json
COMPILE_FLAGS := compile_flags_macos.txt
endif

OBJS := $(patsubst %.c, %.o, $(SRCS))
TEST_OBJS := $(patsubst %.c, %.o, $(TEST_SRCS))

ccodoc: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

ccodoc_test: $(TEST_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

%.o: %.h %.c
	$(CC) $(CFLAGS) -c -o $@ $*.c

.PHONY: run
run: ccodoc
	./ccodoc $(ARGS)

.PHONY: test
test: ccodoc_test
	./ccodoc_test $(ARGS)

# dev
.vscode/settings.json: $(VSCODE_SETTINGS)
	cat $^ > $@

compile_flags.txt: $(COMPILE_FLAGS)
	cat compile_flags_vanilla.txt $^ > $@

.PHONY: clean
clean:
	$(RM) ccodoc ccodoc_test *.o