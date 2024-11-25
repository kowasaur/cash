CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu17 -I/usr/include
LDFLAGS = -lncursesw -ltinfo

.DEFAULT_GOAL := cash

SRCS = main.c deck.c
OBJS = $(SRCS:.c=.o)

cash: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

debug: CFLAGS += -g
debug: clean cash

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) cash
