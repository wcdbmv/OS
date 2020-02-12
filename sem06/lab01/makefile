# Download here: http://www.apuebook.com/code3e.html
# Fix: https://blog.csdn.net/qq_29116917/article/details/103786194
SCADDRESS = ~/Downloads/src.3e/apue.3e
IAPUE = -I $(SCADDRESS)/include/
LAPUE = -L $(SCADDRESS)/lib/ -lapue

LOCKFILE = $(SCADDRESS)/advio/lockfile.o

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

APP = main.out

.PHONY: clean run kill

$(APP): main.o
	$(CC) -o $@ $^

run: $(APP)
	sudo ./$(APP)

kill:
	ps -C $(APP) -o pid= | xargs -- sudo kill

rago13.1.o: rago13.1.c
	$(CC) -o $@ -c $< $(IAPUE)

rago13.2.o: rago13.2.c
	$(CC) -o $@ $(CFLAGS) -c $<

rago13.3.o: rago13.3.c
	$(CC) -o $@ -c $< $(IAPUE)

rago13.3.out: rago13.3.o rago13.2.o
	$(CC) -o $@ $^ $(LOCKFILE) -pthread $(IAPUE) $(LAPUE)

%.o: %.c %.h
	$(CC) -o $@ $(CFLAGS) -c $<

%.o: %.c
	$(CC) -o $@ $(CFALGS) -c $^

clean:
	rm -f *.o *.out 2>/dev/null
