CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu17

.DEFAULT_GOAL := cash

debug: CFLAGS += -g
debug: clean cash

cash: main.c
	$(CC) $(CFLAGS) -o cash main.c 

clean:
	rm -f cash
