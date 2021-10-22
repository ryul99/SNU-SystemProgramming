CC=gcc
CFLAGS=-Wall -O2 -g

SOURCES=$(wildcard *.c)
TARGETS=$(SOURCES:%.c=%)

help:
	@echo "Type make <target> to build a target where <target> is one of: $(TARGETS)"

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)


