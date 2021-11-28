#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define READ   0
#define WRITE  1

void child(int pfd[2])
{
  // close unused pipe end and redirect standard out to write end of pipe
  close(pfd[READ]);
  dup2(pfd[WRITE], STDOUT_FILENO);

  // arguments for execv call
  char *argv[] = {
    "/bin/ls",
    "-l",
    "/etc",
    NULL,
  };

  // exec
  execv(argv[0], argv);

  // if we get here, something went wrong
  exit(EXIT_FAILURE);
}

void parent(int pfd[2])
{
  // close unused pipe end
  close(pfd[WRITE]);

  // read from pipe
  int even = 1;
  char c;
  while (read(pfd[READ], &c, sizeof(c)) == sizeof(c)) {
    // every even character: upper case
    // every odd character: lower case
    if (even) c = toupper(c);
    else c = tolower(c);
    even = !even;

    write(STDOUT_FILENO, &c, sizeof(c));
  }

  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  int pipefd[2];
  pid_t pid;

  if (pipe(pipefd) < 0) {
    printf("Cannot create pipe.\n");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid < 0) {
    printf("Cannot fork.\n");
    exit(EXIT_FAILURE);
  } else if (pid == 0) child(pipefd);
  else parent(pipefd);


  return EXIT_SUCCESS;
}
