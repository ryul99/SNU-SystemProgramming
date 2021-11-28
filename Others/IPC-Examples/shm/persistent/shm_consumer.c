#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "shm_info.h"


int main(int argc, char *argv[])
{
  int shmfd, index;
  shm_msg *msg_buf;

  //
  // setting up shared memory object
  //
  shmfd = shm_open(SHM_IDENTIFIER, O_RDWR, S_IRWXU | S_IRWXG);
  if (shmfd < 0) {
    printf("Cannot open shared memory object.\n");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(shmfd, SHM_SIZE) < 0) {
    printf("Cannot set size of shared memory object.\n");
    exit(EXIT_FAILURE);
  }

  msg_buf = (shm_msg*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (msg_buf == NULL) {
    printf("Cannot map message buffer to shared memory object.\n");
    exit(EXIT_FAILURE);
  }

  //
  // user-defined part: read first unread message from message buffer
  //

  // find first full slot
  index = 0;
  do {
    if (!msg_buf[PREV(index)].unread && msg_buf[index].unread) break;
    index = NEXT(index);
  } while (index != 0);

  if (!msg_buf[index].unread) {
    printf("Message buffer is empty.\n");
  } else {
    printf("Message: %s\n", msg_buf[index].message);
    msg_buf[index].unread = 0;
  }

  //
  // unlink (destroy) shared memory object
  //
  /*
  if (shm_unlink(SHM_IDENTIFIER) < 0) {
    printf("Cannot unlink shared memory object.\n");
    exit(EXIT_FAILURE);
  }
  */

  return EXIT_SUCCESS;
}


