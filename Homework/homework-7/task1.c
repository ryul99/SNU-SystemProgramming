//--------------------------------------------------------------------------------------------------
// System Programming                     Homework #7                                    Fall 2021
//
/// @file
/// @brief template
/// @author Changmin Choi
/// @studid 2017-19841
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void parent(pid_t pid) {
  printf("[%d] Hello from parent.\n", pid);
}

void child(pid_t pid) {
  printf("[%d] Hello from child.\n", pid);
}

/// @brief program entry point
int main(int argc, char *argv[])
{
  pid_t pid = 0;
  pid = fork();
  if (pid == 0) // if child
    child(getpid());
  else if (pid > 0)
    parent(getpid());
  //
  // TODO
  //

  return EXIT_SUCCESS;
}
