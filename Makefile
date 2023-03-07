CC := clang
CFLAGS := -std=c17 -Wall -Wextra -pedantic

ccodoc: main.o ccodoc.o time.o view.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) ccodoc *.o