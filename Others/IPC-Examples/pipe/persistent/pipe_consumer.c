#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "pipe_info.h"

int main(int argc, char *argv[])
{
  FILE *pipe;
  pipe_msg msg;


  //
  // open named pipe
  //
  printf("Opening pipe...\n");
  pipe = fopen(PIPE_IDENTIFIER, "r");
  if (pipe == NULL) {
    printf("Cannot open named pipe.\n");
    exit(EXIT_FAILURE);
  }


  //
  // user-defined part: read messages from pipe
  //

  printf("Waiting for message to arrive. Press Ctrl-C to exit.\n");
  while (1) {
    if (fread(&msg, sizeof(pipe_msg), 1, pipe) == 1) {
      printf("Message received: '%s'\n", msg.message);

      if (strcmp("client::quit", msg.message) == 0) break;
    } else {
      printf("Error reading from pipe.\n");
      exit(EXIT_FAILURE);
    }
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
