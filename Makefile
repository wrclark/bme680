CC=gcc
OPT=-O2 -std=c99 -Wall -Wextra -W -pedantic
CFLAGS=-I. $(OPT)
CFILES=$(wildcard ./*.c)
OBJECTS=$(patsubst %.c,%.o, $(CFILES))
BINARY=bme680

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJECTS) $(BINARY)
