CC=gcc
CFLAGS=-Wall -O2 -g

SOURCES=$(wildcard *.c)
TARGETS=$(SOURCES:%.c=%)

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)


