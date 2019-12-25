CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

RM = rm
LOG_ERR = /dev/null

.PHONY: clean

rw: rw.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	$(RM) -f rw rw.o 2>$(LOG_ERR)
