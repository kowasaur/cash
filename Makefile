CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu17 -I/usr/include
LDFLAGS = -lncursesw -ltinfo

.DEFAULT_GOAL := cash

debug: CFLAGS += -g
debug: clean cash

cash: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o cash main.c 

clean:
	rm -f cash
