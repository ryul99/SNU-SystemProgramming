#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shm_info.h"


int main(int argc, char *argv[])
{
  //
  // unlink (destroy) shared memory object
  //
  if (shm_unlink(SHM_IDENTIFIER) < 0) {
    printf("Cannot unlink shared memory object.\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}


