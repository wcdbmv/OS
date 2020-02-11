# Download here: http://www.apuebook.com/code3e.html
# Fix: https://blog.csdn.net/qq_29116917/article/details/103786194
SCADDRESS = ~/Downloads/src.3e/apue.3e

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

.PHONY: clean

rago13.1.o: rago13.1.c
	$(CC) -o $@ -c $< -I $(SCADDRESS)/include/ -L $(SCADDRESS)/lib/ -lapue

rago13.2.o: rago13.2.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	rm -f *.o 2>/dev/null
