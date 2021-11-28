CFLAGS = -Wall -O2

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: ioredirect

ioredirect: ioredirect.o
	$(CC) $(CFLAGS) -o $@ $@.o

clean:
	rm -f *.o ioredirect 

