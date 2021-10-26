#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXPROC 16

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
  int nproc = atoi(argv[1]);

  if (nproc < 1) nproc = 1;
  if (nproc > MAXPROC) nproc = MAXPROC;

  //
  // TODO
  //


  //
  // that's all, folks!
  //
  return EXIT_SUCCESS;
}
