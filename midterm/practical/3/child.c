#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  if (argc != 2) ABORT("Missing argument.");
  int n = atoi(argv[1]);

  //
  // print message & sleep a bit
  //
  printf("[%d] Hi, this is child %d.\n", getpid(), n);
  fflush(stdout);
  sleep(n > 5 ? 0 : 5 - n);

  //
  // set exit status to child index
  //
  return n;
}
