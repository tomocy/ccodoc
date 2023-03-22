CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic
OBJS := engine.o renderer.o string.o time.o

ifeq ($(PLATFORM), linux)
	CFLAGS += -include ccodoc_macros_linux.h
	OBJS += ccodoc_linux.o
	COMPILE_FLAGS := compile_flags_linux.txt
endif

ifeq ($(PLATFORM), mac)
	CFLAGS += -include ccodoc_macros_mac.h
	OBJS += ccodoc_mac.o
	VSCODE_SETTINGS := .vscode/settings_mac.json
	COMPILE_FLAGS := compile_flags_mac.txt
endif

# ccodoc
ccodoc: main.o $(OBJS)
	$(CC) $(CFLAGS) -lm -lncursesw -o $@ $^

.PHONY: run
run: ccodoc
	./ccodoc $(ARGS)

# test
ccodoc_test: test.o $(OBJS) engine_test.o string_test.o time_test.o
	$(CC) $(CFLAGS) -lm -lncursesw -o $@ $^

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