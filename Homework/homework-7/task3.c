//--------------------------------------------------------------------------------------------------
// System Programming                     Homework #7                                    Fall 2021
//
/// @file
/// @brief template
/// @author <yourname>
/// @studid <studentid>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/// @brief program entry point
int main(int argc, char *argv[])
{
  //
  // TODO
  //
  if (argc < 2) {
    printf("Please give program as argument.\n");
  }
  else {
    if (execvp(argv[1], &argv[1]) < 0) {
      perror("execv() failed");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
