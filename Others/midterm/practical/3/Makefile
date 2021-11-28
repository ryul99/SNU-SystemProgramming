CC=gcc
CFLAGS=-std=c99 -g

TARGETS=child control

.PHONY: clean

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

