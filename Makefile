CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic

ccodoc: main.o ccodoc.o view.o string.o time.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) ccodoc *.o