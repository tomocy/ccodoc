CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic

ccodoc: main.o ccodoc.o renderer.o string.o time.o
	$(CC) $(CFLAGS) -lncursesw -o $@ $^

.PHONY: run
run: ccodoc
	./ccodoc $(ARGS)

.PHONY: test
test: ccodoc
	./ccodoc --test

.PHONY: clean
clean:
	$(RM) ccodoc *.o