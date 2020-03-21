CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

APP = main.out

.PHONY: clean

$(APP): main.o
	$(CC) -o $@ $^

%.o: %.c %.h
	$(CC) -o $@ $(CFLAGS) -c $<

%.o: %.c
	$(CC) -o $@ $(CFALGS) -c $^

clean:
	rm -f *.o *.out 2>/dev/null

