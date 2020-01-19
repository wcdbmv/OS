APP_NAME = app

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

RM = rm
LOG_ERR = /dev/null

.PHONY: clean

$(APP_NAME): main.c
	$(CC) -o $@ $(CFLAGS) $^

clean:
	$(RM) -f $(APP_NAME) 2>$(LOG_ERR)
