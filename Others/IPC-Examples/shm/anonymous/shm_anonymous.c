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
  int index;
  shm_msg *msg_buf;
  ssize_t len = 0;
  size_t buflen = 0;
  char *buf = NULL;
  pid_t pid;

  //
  // create anonymous shared memory object
  //
  msg_buf = (shm_msg*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
  if (msg_buf == NULL) {
    printf("Cannot map message buffer to anonymous shared memory.\n");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid == -1) {
    printf("Can't fork.\n");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    //
    // consumer: read string from shared memory
    //
    pid = getpid();
    printf("Hi, this is the consumer (pid = %d).\n", pid);

    index = 0;
    while (1) {
      if (!msg_buf[index].unread) {
        printf("Message buffer is empty. Waiting...\n");
        while (!msg_buf[index].unread);
        printf("Message has arrvied.\n");
      }

      printf("Message: %s\n\n", msg_buf[index].message);
      msg_buf[index].unread = 0;
      index = NEXT(index);
    }
  } else {
    //
    // producer: read & write string to shared memory
    //
    pid = getpid();
    printf("Hi, this is the producer (pid = %d).\n", pid);

    index = 0;
    while (1) {
      if (msg_buf[index].unread) {
        printf("Message buffer is full. Waiting...\n");
        while (msg_buf[index].unread);
        printf("Message buffer became available.\n");
      }

      printf("Enter message or hit <Enter> to quit: ");
      len = getline(&buf, &buflen, stdin);
      if ((len > 0) && (buf[len-1] == '\n')) buf[len-1] = '\0';

      if (buf[0] == '\0') break;

      msg_buf[index].unread = 1;
      strncpy(msg_buf[index].message, buf, MSG_LENGTH);
      index = NEXT(index);
    }

    free(buf);
  }

  if (munmap(msg_buf, SHM_SIZE) < 0) {
    printf("Cannot unmap shared memory.\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}


