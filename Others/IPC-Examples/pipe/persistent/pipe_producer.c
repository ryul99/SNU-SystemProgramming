#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "pipe_info.h"

int main(int argc, char *argv[])
{
  int res;
  FILE *pipe;
  ssize_t len = 0;
  size_t buflen = 0;
  char *buf = NULL;
  pipe_msg msg;


  //
  // create and open named pipe
  //
  res = mknod(PIPE_IDENTIFIER, S_IFIFO | S_IRWXU | S_IRWXG, 0);
  if ((res < 0) && (errno != EEXIST)) {
    printf("Cannot create named pipe.\n");
    exit(EXIT_FAILURE);
  }

  printf("Opening pipe...\n");
  pipe = fopen(PIPE_IDENTIFIER, "w+");
  if (pipe == NULL) {
    printf("Cannot open named pipe.\n");
    exit(EXIT_FAILURE);
  }


  //
  // user-defined part: write messages into pipe
  //

  while (1) {
    printf("Enter message or hit <Enter> to quit: ");
    len = getline(&buf, &buflen, stdin);
    if ((len > 0) && (buf[len-1] == '\n')) buf[len-1] = '\0';

    if (buf[0] == '\0') break;

    strncpy(msg.message, buf, MSG_LENGTH);
    if (fwrite(&msg, sizeof(pipe_msg), 1, pipe) != 1) {
      printf("Error writing to pipe.\n");
      exit(EXIT_FAILURE);
    }
    fflush(pipe);
  }


  //
  // close named pipe
  //
  if (fclose(pipe) < 0) {
    printf("Cannot close pipe.\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
