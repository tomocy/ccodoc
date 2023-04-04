CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic
LDFLAGS :=
LDLIBS := -lm -lncursesw

LIB_SRCS := ccodoc.c renderer.c canvas.c time.c memory.c string.c math.c platform.c
SRCS := main.c mode.c $(LIB_SRCS)
OBJS := $(patsubst %.c, %.o, $(SRCS))
TEST_SRCS := test.c $(LIB_SRCS) ccodoc_test.c renderer_test.c string_test.c time_test.c platform_test.c
TEST_OBJS := $(patsubst %.c, %.o, $(TEST_SRCS))

override TARGET := $(shell ./tool/build/detect_platform.sh $(TARGET))
ifeq ($(TARGET),)
$(error failed to detect target platform)
endif

ifeq ($(TARGET), linux)
CFLAGS += -include env_linux.h

COMPILE_FLAGS := compile_flags_linux.txt
endif

ifeq ($(TARGET), macos)
CFLAGS += -include env_macos.h

VSCODE_SETTINGS := .vscode/settings_macos.json
COMPILE_FLAGS := compile_flags_macos.txt
endif

ccodoc: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

test.exe: $(TEST_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

assets/sounds/sounds.h: tool/build/embed_sounds.exe assets/sounds/*.mp3
	./$<

%.o: %.h %.c
	$(CC) $(CFLAGS) -c -o $@ $*.c

%.exe: %.c
	$(CC) $(CFLAGS) -o $@ $*.c

.PHONY: run
run: ccodoc
	./ccodoc $(ARGS)

.PHONY: test
test: test.exe
	./$< $(ARGS)

.PHONY: clean
clean:
	$(RM) ccodoc
	find . -name '*.o' -or -name '*.exe' | while read -r file; do $(RM) "$${file}"; done

# dev
.vscode/settings.json: $(VSCODE_SETTINGS)
	cat $^ > $@

compile_flags.txt: $(COMPILE_FLAGS)
	cat compile_flags_vanilla.txt $^ > $@
