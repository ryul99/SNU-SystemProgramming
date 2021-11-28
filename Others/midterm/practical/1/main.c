#include <stdio.h>
#include <stdlib.h>

#include "fib.h"

// abort process with an optional error message
void ABORT(char *msg)
{
  if (msg) { printf("%s\n", msg); fflush(stdout); }
  abort();
}

int main(int argc, char *argv[])
{
  //
  // convert argument at command line into number
  //
  if (argc != 2) ABORT("Invalid number of arguments.");
  long int n = atol(argv[1]);

  //
  // compute n-th Fibonnaci number & print result
  //
  long int fib = fibonacci(n);
  printf("fibonnaci(%ld) = %ld\n", n, fib);

  //
  // that's all, folks!
  //
  return EXIT_SUCCESS;
}
