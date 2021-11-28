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
  //
  // open directory stream and retrieve directory descriptor
  //
  DIR *d = opendir(dn);
  if (d == NULL) ABORT(strerror(errno));
  int dd = dirfd(d);

  //
  // iterate through directory
  //
  size_t size = 0;

  struct dirent *entry;
  struct stat sb;
  while ((entry = getNext(d)) != NULL) {
    //
    // retrieve metadata using fstatat()
    //
    if (fstatat(dd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == 0) {
      //
      // if it's a file, add its size to the total size
      //
      if (S_ISREG(sb.st_mode)) size += sb.st_size;

      //
      // if it's a directory, compose the pathname of the directory,
      // enumerate it, and add its size to the total size
      //
      if (S_ISDIR(sb.st_mode)) {
        char *subdir;

        if (asprintf(&subdir, "%s/%s", dn, entry->d_name) < 0) ABORT(strerror(errno));

        size += dirsize(subdir);

        free(subdir);
      }

    } else {
      ABORT(strerror(errno));
    }

  }

  //
  // close directory
  //
  closedir(d);

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
