CC := clang
CFLAGS := -Wall -Wextra -pedantic -std=c17

ccodoc: main.o
	$(CC) $(CFLAGS) -o $@ main.o

.PHONY: clean
clean:
	$(RM) ccodoc *.o