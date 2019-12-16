CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

RM = rm
LOG_ERR = /dev/null

OBJS = children.o

.PHONY: all

all: pc rw

pc: pc.o $(OBJS)
	$(CC) -o $@ $^

rw: rw.o $(OBJS)
	$(CC) -o $@ $^

%.o: %.c %.h
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	$(RM) -f pc rw *.o 2>$(LOG_ERR)
