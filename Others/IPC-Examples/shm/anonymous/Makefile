CFLAGS = -Wall -O2


DEPS = shm_info.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: shm_anonymous shm_anonymous2

shm_anonymous: shm_anonymous.o
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

shm_anonymous2: shm_anonymous2.o
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

clean:
	rm -f *.o shm_anonymous shm_anonymous2

