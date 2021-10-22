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
#include <wait.h>
#include <signal.h>

static int counter = 0;

void parent(pid_t pid, pid_t child_pid) {
  int wstatus = 0;
  printf("[%d] Hello from parent.\n", pid);
  printf("[%d]   Waiting for child to terminate...\n", pid);
  waitpid(child_pid, &wstatus, 0);
  if (WIFEXITED(wstatus)) {
    printf("[%d] Child has terminated normally. "
           "It has received %d SIGUSR1 signals.\n", pid, WEXITSTATUS(wstatus));
  }
  else {
    printf("[%d] Child has terminated abnormally. "
           "It has received %d SIGUSR1 signals.\n", pid, WEXITSTATUS(wstatus));
  }
}

void hdl_sigusr1(int sig) {
  counter++;
  printf("[%d] Child received SIGUSR1! Count = %d.\n", getpid(), counter);
}

void hdl_sigusr2(int sig) {
  exit(counter);
}

void child(pid_t pid) {
  struct sigaction action1;
  struct sigaction action2;
  action1.sa_handler = hdl_sigusr1;
  action2.sa_handler = hdl_sigusr2;
  printf("[%d] Hello from child.\n", pid);
  sigaction(SIGUSR1, &action1, NULL);
  printf("[%d]   SIGUSR1 handler installed.\n", pid);
  sigaction(SIGUSR2, &action2, NULL);
  printf("[%d]   SIGUSR2 handler installed.\n", pid);
  printf("[%d]   Waiting for signals...\n", pid);
  while(1);
}

/// @brief program entry point
int main(int argc, char *argv[])
{
  pid_t pid = 0;
  pid = fork();
  if (pid == 0) // if child
    child(getpid());
  else if (pid > 0)
    parent(getpid(), pid);
  //
  // TODO
  //

  return EXIT_SUCCESS;
}
