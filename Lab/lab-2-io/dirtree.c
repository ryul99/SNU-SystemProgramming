//--------------------------------------------------------------------------------------------------
// System Programming                         I/O Lab                                    Fall 2021
//
/// @file
/// @brief resursively traverse directory tree and list all entries
/// @author Changmin Choi
/// @studid 2017-19841
//--------------------------------------------------------------------------------------------------

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <grp.h>
#include <pwd.h>

#define MAX_DIR 64            ///< maximum number of directories supported

/// @brief output control flags
#define F_TREE      0x1       ///< enable tree view
#define F_SUMMARY   0x2       ///< enable summary
#define F_VERBOSE   0x4       ///< turn on verbose mode

/// @brief struct holding the summary
struct summary {
  unsigned int dirs;          ///< number of directories encountered
  unsigned int files;         ///< number of files
  unsigned int links;         ///< number of links
  unsigned int fifos;         ///< number of pipes
  unsigned int socks;         ///< number of sockets

  unsigned long long size;    ///< total size (in bytes)
  unsigned long long blocks;  ///< total number of blocks (512 byte blocks)
};


/// @brief abort the program with EXIT_FAILURE and an optional error message
///
/// @param msg optional error message or NULL
void panic(const char *msg)
{
  if (msg) fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}


/// @brief read next directory entry from open directory 'dir'. Ignores '.' and '..' entries
///
/// @param dir open DIR* stream
/// @retval entry on success
/// @retval NULL on error or if there are no more entries
struct dirent *getNext(DIR *dir)
{
  struct dirent *next;
  int ignore;

  do {
    errno = 0;
    next = readdir(dir);
    if (errno != 0) perror(NULL);
    ignore = next && ((strcmp(next->d_name, ".") == 0) || (strcmp(next->d_name, "..") == 0));
  } while (next && ignore);

  return next;
}


/// @brief qsort comparator to sort directory entries. Sorted by name, directories first.
///
/// @param a pointer to first entry
/// @param b pointer to second entry
/// @retval -1 if a<b
/// @retval 0  if a==b
/// @retval 1  if a>b
static int dirent_compare(const void *a, const void *b)
{
  struct dirent *e1 = *((struct dirent**)a);
  struct dirent *e2 = *((struct dirent**)b);

  // if one of the entries is a directory, it comes first
  if (e1->d_type != e2->d_type) {
    if (e1->d_type == DT_DIR) return -1;
    if (e2->d_type == DT_DIR) return 1;
  }

  // otherwise sorty by name
  return strcmp(e1->d_name, e2->d_name);
}


/// @brief recursively process directory @a dn and print its tree
///
/// @param dn absolute or relative path string
/// @param pstr prefix string printed in front of each entry
/// @param stats pointer to statistics
/// @param flags output control flags (F_*)
void processDir(const char *dn, const char *pstr, struct summary *stats, unsigned int flags)
{
  // TODO
  // Errors
  char *error_oom, *pstr_err;
  if (flags & F_TREE)
    pstr_err = "`-";
  else
    pstr_err = "  ";
  asprintf(&error_oom, "%s%sERROR: Cannot allocate memory\n", pstr, pstr_err);
  errno = 0;
  // open dir
  DIR *d = opendir(dn);
  if (errno == EACCES) {
    printf("%s%sERROR: Permission denied\n", pstr, pstr_err);
    errno = 0;
    return;
  }
  else if (errno == ENOENT) {
    printf("%s%sERROR: No such file or directory\n", pstr, pstr_err);
    errno = 0;
    return;
  }
  else if (errno == ENOMEM) {
    printf("%s", error_oom);
    errno = 0;
    return;
  }
  else if (errno == ENOTDIR) {
    printf("%s%sERROR: Not a directory\n", pstr, pstr_err);
    errno = 0;
    return;
  }
  else if (errno != 0) {
    errno = 0;
    return;
  }
  // read dirents
  int num_child = 0;
  struct dirent **children = (struct dirent **)malloc(MAX_DIR * sizeof(struct dirent*));
  if (children == NULL) {
    printf("%s", error_oom);
    closedir(d);
    errno = 0;
    return;
  }
  // loop reading dirent
  struct dirent *child;
  while ((child = getNext(d))) {
    children[num_child] = (struct dirent*) malloc(child->d_reclen);
    if (children[num_child] == NULL) {
      printf("%s", error_oom);
      for (int i = 0; i < num_child; i++) {
        free(children[i]);
      }
      free(children);
      closedir(d);
      errno = 0;
      return;
    }
    memcpy(children[num_child], child, child->d_reclen);
    num_child++;
    if (num_child >= MAX_DIR) {
      children = (struct dirent **)realloc(children, (num_child + MAX_DIR) * sizeof(struct dirent*));
      if (children == NULL) {
        printf("%s", error_oom);
        for (int i = 0; i < num_child; i++) {
          free(children[i]);
        }
        free(children);
        closedir(d);
        errno = 0;
        return;
      }
    }
  }
  // sort dirents
  qsort(children, num_child, sizeof(struct dirent*), dirent_compare);
  // process dirent
  for (int i = 0; i < num_child; i++) {
    unsigned char child_dtype = ' ';
    char *child_dname = children[i]->d_name;

    char *new_pstr;
    char *name;
    struct stat child_stat;
    char *child_dir;
    asprintf(&child_dir, "%s/%s", dn, child_dname);

    // fancy tree view mode
    if (flags & F_TREE) {
      if (i < num_child - 1) {
        // not the last child
        asprintf(&name, "%s|-%s", pstr, child_dname);
        // set new_pstr
        asprintf(&new_pstr, "%s| ", pstr);
      }
      else {
        // the last child
        asprintf(&name, "%s`-%s", pstr, child_dname);
        // set new_pstr
        asprintf(&new_pstr, "%s  ", pstr);\
      }
    }
    // simple mode
    else {
      asprintf(&name, "%s  %s", pstr, child_dname);
      //set new_pstr
      asprintf(&new_pstr, "%s  ", pstr);
    }
    if (flags & F_VERBOSE && strlen(name) > 54) {
      char tmp[60];
      strncpy(tmp, name, 51 * sizeof(char));
      tmp[51] = '\0';
      asprintf(&name, "%s...", tmp);
    }
    // update summary
    if (children[i]->d_type == DT_REG) {
      (stats->files)++;
      child_dtype = ' ';
    }
    else if (children[i]->d_type == DT_DIR) {
      (stats->dirs)++;
      child_dtype = 'd';
    }
    else if (children[i]->d_type == DT_FIFO) {
      (stats->fifos)++;
      child_dtype = 'f';
    }
    else if (children[i]->d_type == DT_SOCK) {
      (stats->socks)++;
      child_dtype = 's';
    }
    else if (children[i]->d_type == DT_LNK) {
      (stats->links)++;
      child_dtype = 'l';
    }
    else if (children[i]->d_type == DT_CHR) {
      child_dtype = 'c';
    }
    else if (children[i]->d_type == DT_BLK) {
      child_dtype = 'b';
    }
    lstat(child_dir, &child_stat);
    if (errno != 0) {
      printf("%-54s  %s\n", name, "No such file or directory");
      errno = 0;
    }
    else {
      stats->size += child_stat.st_size;
      stats->blocks += child_stat.st_blocks;

      // print child
      if (flags & F_VERBOSE) {
        char *username = strdup((getpwuid(child_stat.st_uid)->pw_name));
        char *groupname = strdup((getgrgid(child_stat.st_gid)->gr_name));
        printf("%-54s  %8s:%-8s  %10ld  %8ld  %1c\n",
            name,
            username,
            groupname,
            child_stat.st_size,
            child_stat.st_blocks,
            child_dtype
        );
      }
      else {
        printf("%s\n", name);
      }
    }
    // recursive
    if (children[i]->d_type == DT_DIR) {
      processDir(child_dir, new_pstr, stats, flags);
    }
  }
  // finish
  for (int i = 0; i < num_child; i++) {
    free(children[i]);
  }
  free(children);
  closedir(d);
  return;
}


/// @brief print program syntax and an optional error message. Aborts the program with EXIT_FAILURE
///
/// @param argv0 command line argument 0 (executable)
/// @param error optional error (format) string (printf format) or NULL
/// @param ... parameter to the error format string
void syntax(const char *argv0, const char *error, ...)
{
  if (error) {
    va_list ap;

    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);

    printf("\n\n");
  }

  assert(argv0 != NULL);

  fprintf(stderr, "Usage %s [-t] [-s] [-v] [-h] [path...]\n"
                  "Gather information about directory trees. If no path is given, the current directory\n"
                  "is analyzed.\n"
                  "\n"
                  "Options:\n"
                  " -t        print the directory tree (default if no other option specified)\n"
                  " -s        print summary of directories (total number of files, total file size, etc)\n"
                  " -v        print detailed information for each file. Turns on tree view.\n"
                  " -h        print this help\n"
                  " path...   list of space-separated paths (max %d). Default is the current directory.\n",
                  basename(argv0), MAX_DIR);

  exit(EXIT_FAILURE);
}


/// @brief program entry point
int main(int argc, char *argv[])
{
  //
  // default directory is the current directory (".")
  //
  const char CURDIR[] = ".";
  const char *directories[MAX_DIR];
  int   ndir = 0;

  struct summary dstat, tstat;
  unsigned int flags = 0;

  //
  // parse arguments
  //
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      // format: "-<flag>"
      if      (!strcmp(argv[i], "-t")) flags |= F_TREE;
      else if (!strcmp(argv[i], "-s")) flags |= F_SUMMARY;
      else if (!strcmp(argv[i], "-v")) flags |= F_VERBOSE;
      else if (!strcmp(argv[i], "-h")) syntax(argv[0], NULL);
      else syntax(argv[0], "Unrecognized option '%s'.", argv[i]);
    } else {
      // anything else is recognized as a directory
      if (ndir < MAX_DIR) {
        directories[ndir++] = argv[i];
      } else {
        printf("Warning: maximum number of directories exceeded, ignoring '%s'.\n", argv[i]);
      }
    }
  }

  // if no directory was specified, use the current directory
  if (ndir == 0) directories[ndir++] = CURDIR;


  //
  // process each directory
  //
  // TODO
  tstat = (struct summary) {.blocks=0, .dirs=0, .fifos=0, .files=0, .links=0, .size=0, .socks=0};
  for (int i = 0; i < ndir; i++) {
    dstat = (struct summary) {.blocks=0, .dirs=0, .fifos=0, .files=0, .links=0, .size=0, .socks=0};
    if (flags & F_SUMMARY) {
      if (flags & F_VERBOSE) {
        printf("%-60s%-10s %14s%15s \n", "Name", "User:Group", "Size", "Blocks Type");
      }
      else {
        printf("Name\n");
      }
      printf("----------------------------------------------------------------------------------------------------\n");
    }
    printf("%s\n", directories[i]);
    processDir(directories[i], "", &dstat, flags);
    if (flags & F_SUMMARY)
      printf("----------------------------------------------------------------------------------------------------\n");

    // print summary
    if (flags & F_SUMMARY) {
      char *str_file, *str_dir, *str_link, *str_pipe, *str_sock, *str_summary;
      //
      str_file = dstat.files == 1 ? "file" : "files";
      str_dir = dstat.dirs == 1 ? "directory" : "directories";
      str_link = dstat.links == 1 ? "link" : "links";
      str_pipe = dstat.fifos == 1 ? "pipe" : "pipes";
      str_sock = dstat.socks == 1 ? "socket" : "sockets";
      asprintf(&str_summary, "%d %s, %d %s, %d %s, %d %s, and %d %s",
          dstat.files, str_file,
          dstat.dirs, str_dir,
          dstat.links, str_link,
          dstat.fifos, str_pipe,
          dstat.socks, str_sock
      );
      if (flags & F_VERBOSE)
        printf("%-68s   %14lld %9lld\n", str_summary, dstat.size, dstat.blocks);
      else
        printf("%s\n", str_summary);
      printf("\n");
    }

    // aggregate summary
    tstat.blocks += dstat.blocks;
    tstat.dirs += dstat.dirs;
    tstat.fifos += dstat.fifos;
    tstat.files += dstat.files;
    tstat.links += dstat.links;
    tstat.size += dstat.size;
    tstat.socks += dstat.socks;
  }
  //
  // print grand total
  //
  if ((flags & F_SUMMARY) && (ndir > 1)) {
    printf("Analyzed %d directories:\n"
           "  total # of files:        %16d\n"
           "  total # of directories:  %16d\n"
           "  total # of links:        %16d\n"
           "  total # of pipes:        %16d\n"
           "  total # of socksets:     %16d\n",
           ndir, tstat.files, tstat.dirs, tstat.links, tstat.fifos, tstat.socks);

    if (flags & F_VERBOSE) {
      printf("  total file size:         %16llu\n"
             "  total # of blocks:       %16llu\n",
             tstat.size, tstat.blocks);
    }

  }

  //
  // that's all, folks
  //
  return EXIT_SUCCESS;
}
