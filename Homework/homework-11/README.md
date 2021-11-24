# Homework 11 - CRC Computation with Threads

In this homework, you learn how to compute a CRC using multiple threads.

[[_TOC_]]



## Before you begin

For to fork this project into your namespace and verify that its visibility is set to *private*.



## Description

We want to write a program that computes the 8-bit CRC of a file using multiple threads. After the file has been mapped into the process' address space, from 1 to 256 threads jointly compute its CRC. Each thread should process the same amount of work (except for differences with rounding).

We will use two different methods to do so: once by using the thread routine's return value, once using a global variable.

## Skeleton code

We provide skeleton code for both versions. Most of the code is already there so that you can focus on the relevant parts.

### Creating and joining threads
Common to both tasks is the thread creation and waiting for the threads to join. You will find the relevant code in the `main()` function.
```C
  //
  // create 'nthread' threads, each computing a part of the CRC
  //
  ThreadData *td = malloc(nthread * sizeof(ThreadData)); assert(td != NULL);

  // TODO

  //
  // wait for all threads to end and sum up their result
  //
  unsigned char crc = 0;

  // TODO
```

The variable `td` of type `ThreadData` is used to pass the necessary information to the individual threads. `ThreadData` is defined at the beginning of the code as follows:
```C
typedef struct _tdata {
  pthread_t tid;
  void *data;
  off_t from, to;
} ThreadData;
```

The field `tid` is meant to hold a thread's ID. A reference to it should be given to the `pthread_create()` routine
```C
  for (int i...) {
    int res = pthread_create(&td[i].tid, ...)
  }
```
You can then use the stored thread IDs to wait for all threads to complete using `pthread_join()`.

The field `data` should hold a pointer to the file data. The skeleton code already maps the file into the process' address space and obtains a pointer to the file data (`filedata`).

The fields `from` and `to`, finally, indicate the start and end position of the data the thread in question should compute. Try to balance the size of work chunks as much as possible.


### Using the thread return value

After computing the CRC for the assigned work chunk in the thread routine `crc8()`, the thread-local crc8 variable has to be passed back to the main thread where it is aggregated to form the total CRC. 

In this first task, you need to return that value using the thread routine's `return` statement or by extending the `ThreadData` struct and pass the value in there. You can bypass the type system and pass the character as a pointer or allocate a block on the heap as we have seen in the lecture.

Modify the file `parallel.rval.c` to implement your solution to this task.


### Using a global variable

In this second task, you are to update the global variable `crc` directly in the thread routine. Since this variable is shared and updated by different threads, we have to make sure accesses to it are protected with a semaphore. The semaphore `mutex` has already been defined (right next to the global `crc`) at the beginning of the skeleton code.

You need to (1) insert code that initializes the semaphore properly (have a look at `sem_init()`) and then (2) protect accesses to the global variable using the `P()/V()` equivalents of the Pthreads library.

Modify the file `parallel.semaphore.c` to implement your solution to this task.


### Logistics

Modify the skeleton provided in `parallel.*.c` and use GNU Make to compile it. Both programs take a filename and the number of threads as command line arguments. We do not provide a reference implementation - the CRC of the included data file `data` is 88.

```bash
$ vi parallel.rval.c
$ vi parallel.semaphore.c
$ make
gcc -Wall -O2 -g -pthread -o parallel.rval parallel.rval.c
gcc -Wall -O2 -g -pthread -o parallel.semaphore parallel.semaphore.c
$ ./parallel.rval
Error: Missing arguments.

Compute 8-bit CRC for a file using n threads.

Syntax: parallel.rval <filename> <threads>

  <filename>    name of file
  <threads>     number of threads (1-256)

Aborted
$ ./parallel.rval data 1
CRC8(data, 1) = 88
$ ./parallel.rval data 16
CRC8(data, 16) = 88
$ ./parallel.rval data 256
CRC8(data, 256) = 88
$ ./parallel.semaphore data 17
CRC8(data, 17) = 88
```



## Submission

As usual, commit your code frequently and submit your repository to the Gitlab server before the submission deadline.
