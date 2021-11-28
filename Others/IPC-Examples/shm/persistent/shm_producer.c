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
  int shmfd, index, initialize = 0;
  shm_msg *msg_buf;
  ssize_t len = 0;
  size_t buflen = 0;
  char *buf = NULL;

  //
  // setting up shared memory object (create if necessary)
  //
  shmfd = shm_open(SHM_IDENTIFIER, O_RDWR, S_IRWXU | S_IRWXG);
  if (shmfd < 0) {
    // call may have failed because the object did not exist yet
    shmfd = shm_open(SHM_IDENTIFIER, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0) {
      printf("Cannot create shared memory object.\n");
      exit(EXIT_FAILURE);
    }

    initialize = 1;
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

  if (initialize) {
    for (index = 0; index < MAX_MESSAGES; index++) {
      msg_buf[index].unread = 0;
    }
  }

  //
  // user-defined part: write messages into shared message buffer
  //

  // find first empty slot
  index = 0;
  do {
    if (msg_buf[PREV(index)].unread && !msg_buf[index].unread) break;
    index = NEXT(index);
  } while (index != 0);

  while (1) {
    if (msg_buf[index].unread) {
      printf("Message buffer is full.\n");
      break;
    }

    printf("Enter message or hit <Enter> to quit: ");
    len = getline(&buf, &buflen, stdin);
    if ((len > 0) && (buf[len-1] == '\n')) buf[len-1] = '\0';

    if (buf[0] == '\0') break;

    msg_buf[index].unread = 1;
    strncpy(msg_buf[index].message, buf, MSG_LENGTH);
    index = NEXT(index);
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


