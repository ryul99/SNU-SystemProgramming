//--------------------------------------------------------------------------------------------------
// System Programming                     Homework #2                                    Fall 2021
//
/// @file
/// @brief main file
/// @author Changmin Choi
/// @studid 2017-19841
#include <stdio.h>
#include <stdlib.h>
#include "mathlib.h"


/// @brief program entry point
int main(int argc, char *argv[])
{
  // first print all command line arguments
  printf("Command line arguments (%d total):\n", argc);
  for(int i = 0; i < argc; i++){
    printf("   %d: '%s'\n", i, argv[i]);
  }

  printf("\n");
  // if there is at least one command line argument,
  // add them all up and print the sum.
  int s = 0;
  for(int i = 1; i < argc; i++){
    s = add(s, atoi(argv[i]));
  }
  printf("Sum of arguments: %d\n", s);

  return EXIT_SUCCESS;
}
