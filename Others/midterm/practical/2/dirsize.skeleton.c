#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// abort process with an optional error message
void ABORT(char *msg)
{
  if (msg) { printf("%s\n", msg); fflush(stdout); }
  abort();
}

// read next directory entry from open directory 'dir'. Ignores '.' and '..' entries
struct dirent *getNext(DIR *dir)
{
  struct dirent *next;
  int ignore;

  do {
    errno = 0;
    next = readdir(dir);
    if (errno != 0) ABORT(strerror(errno));
    ignore = next && ((strcmp(next->d_name, ".") == 0) || (strcmp(next->d_name, "..") == 0));
  } while (next && ignore);

  return next;
}

size_t dirsize(char *dn)
{
  size_t size = 0;

  //
  // TODO
  //

  //
  // return size
  //
  return size;
}


int main(int argc, char *argv[])
{
  //
  // use directory provided on command line or '.' if none given
  //
  char *dir = argc > 1 ? argv[1] : ".";

  //
  // let the magic work...
  //
  printf("Computing size of '%s'\n", dir); fflush(stdout);
  size_t s = dirsize(dir);
  printf("  size: %lu bytes\n", s);

  //
  // that's all, folks!
  //
  return 0;
}
