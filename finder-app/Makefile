CC = $(CROSS_COMPILE)gcc
CFLAGS ?= -Wall -Wextra -Werror -std=c11

.PHONY: all clean

all: writer

writer: writer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

writer.o: writer.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) writer *.o
