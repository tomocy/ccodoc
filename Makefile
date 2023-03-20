CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic

OBJS := engine.o renderer.o string.o time.o

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

# utils
.PHONY: clean
clean:
	$(RM) ccodoc ccodoc_test *.o