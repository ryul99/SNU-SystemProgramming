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
  // create nproc children and execute the 'child' binary
  //
  for (int i=0; i<nproc; i++) {
    pid_t pid = fork();
    if (pid == -1) ABORT(strerror(errno));
    else if (pid == 0) {
      //
      // child
      //

      // the program 'child' expects the child's index
      // (as a string) on the command line:
      //
      // $ ./child 1
      // $ ./child 2

      char *prog = "./child";
      char *index;
      if (asprintf(&index, "%d", i+1) < 0) ABORT(strerror(errno));

      //
      // execute program
      //
      execlp(prog, prog, index, NULL);

      //
      // we only come back here if something went wrong
      //
      ABORT(strerror(errno));
    }
  }

  //
  // wait for children to terminate & check status
  //
  for (int i=0; i<nproc; i++) {
    int status;
    pid_t pid = wait(&status);

    if (pid > 0) {
      printf("Child %d terminated %snormally", pid, WIFEXITED(status) ? "" : "ab");
      if (WIFEXITED(status)) printf(" with exit code %d", WEXITSTATUS(status));
      printf(".\n");
    }
  }

  //
  // that's all, folks!
  //
  return EXIT_SUCCESS;
}
