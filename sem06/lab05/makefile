CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-format-truncation

SRC_EXT = c
EXE_EXT = out

SRCS = $(wildcard *.$(SRC_EXT))
EXES = $(patsubst %.$(SRC_EXT), %.$(EXE_EXT), $(SRCS))

.PHONY: all clean

all: $(EXES)

%.$(EXE_EXT): %.$(SRC_EXT)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.$(EXE_EXT) 2>/dev/null
