CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

RM = rm
LOG_ERR = /dev/null

.PHONY: clean

rw: rw.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) -f rw 2>$(LOG_ERR)
