APP_NAME = app

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

RM = rm
LOG_ERR = /dev/null

$(APP_NAME)%: main%.o display.o message.o
	$(CC) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -f $(APP_NAME)* *.o 2>$(LOG_ERR)
