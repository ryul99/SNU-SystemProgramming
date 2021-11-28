#ifndef __PIPE_INFO__
#define __PIPE_INFO__

#define PIPE_IDENTIFIER  "./M1522.000800"

#define MSG_LENGTH 60

typedef struct _pipe_msg {
  char message[MSG_LENGTH];
} pipe_msg;

#endif
