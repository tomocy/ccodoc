CC := clang
CFLAGS := -Wall -Wextra -pedantic -std=c17

CCODOC: main.o
	$(CC) $(CFLAGS) -o $@ main.o

.PHONY: clean
clean:
	$(RM) CCODOC *.o