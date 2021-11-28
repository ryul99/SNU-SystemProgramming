CFLAGS = -Wall -O2


DEPS = shm_info.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: shm_producer shm_consumer shm_dump shm_remove

shm_producer: shm_producer.o
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

shm_consumer: shm_consumer.o
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

shm_dump: shm_dump.o
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

shm_remove: shm_remove.o
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

clean:
	rm -f *.o shm_producer shm_consumer shm_dump shm_remove

