CC := clang
CFLAGS := -Wall -Wextra -pedantic -std=c17

ccodoc: main.o ccodoc.o time.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) ccodoc *.o