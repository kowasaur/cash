CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu17

LDFLAGS = -lncurses

.DEFAULT_GOAL := cash

debug: CFLAGS += -g
debug: clean cash

cash: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o cash main.c 

clean:
	rm -f cash
