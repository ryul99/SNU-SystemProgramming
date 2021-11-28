CFLAGS = -Wall -O2


DEPS = pipe_info.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: pipe_producer pipe_consumer

pipe_producer: pipe_producer.o
	$(CC) $(CFLAGS) -o $@ $@.o

pipe_consumer: pipe_consumer.o
	$(CC) $(CFLAGS) -o $@ $@.o

clean:
	rm -f *.o pipe_producer pipe_consumer

