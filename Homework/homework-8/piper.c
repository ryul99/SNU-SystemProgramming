//--------------------------------------------------------------------------------------------------
// System Programming                     Homework #8                                    Fall 2021
//
/// @file
/// @brief Summarize size of files in a directory tree
/// @author Changmin Choi
/// @studid 2017-19841

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

/// @brief program entry point
int main(int argc, char *argv[])
{
  char *dir = (argc > 1 ? argv[1] : ".");

  //
  // TODO
  //

  int pipefd[2];
  pid_t cpid;

  if (pipe(pipefd) == -1) {
    perror("pipe failed\n");
    return EXIT_FAILURE;
  }

  cpid = fork();
  if (cpid == -1) {
    perror("fork failed\n");
    return EXIT_FAILURE;
  }

  if (cpid == 0) { // child
    close(pipefd[READ]);
    if (dup2(pipefd[WRITE], STDOUT_FILENO) < 0) {
      perror("dup2 failed\n");
      return EXIT_FAILURE;
    }

    char* child_argv[7];
    child_argv[0] = "find";
    child_argv[1] = dir;
    child_argv[2] = "-type";
    child_argv[3] = "f";
    child_argv[4] = "-printf";
    child_argv[5] = "%s %f\n";
    child_argv[6] = NULL;
    if (execvp(child_argv[0], child_argv) < 0) {
      perror("exec find failed\n");
      return EXIT_FAILURE;
    }
    close(pipefd[WRITE]);
    return EXIT_SUCCESS;
  }
  else if (cpid > 0) {
    close(pipefd[WRITE]);
    FILE* pipe_file = fdopen(pipefd[READ], "r");
    if (pipe_file == NULL) {
      perror("Cannot open pipe\n");
      return EXIT_FAILURE;
    }

    long long num_file = 0;
    char* buf;
    char* biggest_file_name = NULL;
    long long size;
    long long total_size = 0;
    long long biggest_file_size = 0;
    while (fscanf(pipe_file, "%lld %ms", &size, &buf) != EOF){
      num_file++;
      total_size += size;
      if (biggest_file_size <= size) {
        biggest_file_size = size;
        biggest_file_name = buf;
      }
    }
    printf("Found %lld files with a total size of %lld bytes.\n"
           "The largest file is \'%s\' with a size of %lld bytes.\n",
           num_file, total_size, biggest_file_name, biggest_file_size);
    close(pipefd[READ]);
    return EXIT_SUCCESS;
  }

  //
  // That's all, folks!
  //
  return EXIT_SUCCESS;
}
