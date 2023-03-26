CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic
LDLIBS := -lm -lncursesw

LIB_SRCS := engine.c renderer.c string.c time.c math.c
SRCS := main.c $(LIB_SRCS) canvas.c
TEST_SRCS := test.c $(LIB_SRCS) canvas.c engine_test.c renderer_test.c string_test.c time_test.c

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

# ccodoc
ccodoc: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

.PHONY: run
run: ccodoc
	./ccodoc $(ARGS)

# test
ccodoc_test: $(TEST_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

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