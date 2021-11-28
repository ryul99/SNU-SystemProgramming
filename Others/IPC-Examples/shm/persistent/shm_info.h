#ifndef __SHM_INFO__
#define __SHM_INFO__

#define SHM_IDENTIFIER  "M1522.000800"

#define MSG_LENGTH 60
#define MAX_MESSAGES 8

#define PREV(idx) (idx > 0 ? idx-1 : MAX_MESSAGES-1)
#define NEXT(idx) (idx < MAX_MESSAGES-1 ? idx+1 : 0)

typedef struct _shm_msg {
  int unread;
  char message[MSG_LENGTH];
} shm_msg;


#define SHM_SIZE (sizeof(shm_msg) * MAX_MESSAGES)

#endif
