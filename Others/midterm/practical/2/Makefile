CC=gcc
CFLAGS=-std=c99 -g

TARGET=dirsize

.PHONY: clean

all: $(TARGET)

%: %.c
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(TARGET)
