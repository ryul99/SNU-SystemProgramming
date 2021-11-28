CFLAGS = -Wall -O2


DEPS = pipe_info.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: pipe_anonymous

pipe_anonymous: pipe_anonymous.o
	$(CC) $(CFLAGS) -o $@ $@.o

clean:
	rm -f *.o pipe_anonymous

