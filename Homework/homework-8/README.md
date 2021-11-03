# Homework 8 - Interprocess Communication

In this homework, you learn how to
*  create a pipe between a parent and a child
*  how to use Standard I/O on pipes

[[_TOC_]]



## Before you begin

Do not forget to fork this project into your namespace and verify that its visibility is set to *private*.



## Description

We want to write a program that computes the size of all files in a directory tree. Since we are well familiar with Unix philosophy, we do not want to write the directory traversal ourselves, but rather invoke an existing tool that can return a list of all files along with their size. In class, we have learned about `fork()`, `exec()`, and `pipe()` and want to implement the program using these techniques.


## Listing all files and their size

We will use the `find` tool to search for files and display their sizes. `find` is a very powerful tool with a large number of options. To get the size in bytes and the name of all files in a directory tree, one would write
```bash
$ find tree/ -type f -printf "%s %f\n"
6857 f3
3957 f11
19478 f12
7840 f114
28851 f112
...
17880 f311
9605 f312
$
```

## Piper

Our program is called `piper`. A Makefile and a skeleton (`piper.c`) are provided.

Piper works as follows:
1. Read directory from command line. If none provided, use the current directory.
2. Create a pipe
3. Fork into a parent and child process

The child process then performs the following actions:
1. Redirect standard output to the write-end of the pipe
2. Execute `find` with the necessary command line arguments

The parent, on the other hand:
1. Open a FILE stream on the read end of the pipe (use the `fdopen()` Glibc call)
2. Read tuples <size filename> from the stream until no more tuples are available. The `fscanf()` function will be very useful here. We recommend using the `"%ms%` modifier to allocate & read a string into a buffer (see manpage).
3. Print out statistics:  
   * total number of files and total size of all files
   * the name and the size of the largest file encountered


Hint: make sure to close unused pipe endpoints or else your parent process may wait forever for more data to arrive.

### Logistics

Modify the skeleton provided in `piper.c` and use GNU Make to compile it. You can test the output on the included `tree/` directory.
```bash
$ vi piper.c
$ make
gcc -Wall -O2 -g -o piper piper.c
$ ./piper tree/
Found 26 files with a total size of 402569 bytes.
The largest file is 'f22' with a size of 31203 bytes.
$
```

As usual, commit your code frequently.
```bash
$ git add piper.c
$ git commit -m "Redirect stdout to pipe in child."
```



## Submission

Submit your repository to the Gitlab server before the submission deadline.
```bash
$ git push
Enumerating objects: 13, done.
Counting objects: 100% (13/13), done.
Delta compression using up to 4 threads
Compressing objects: 100% (5/5), done.
Writing objects: 100% (7/7), 500 bytes | 500.00 KiB/s, done.
Total 7 (delta 4), reused 0 (delta 0), pack-reused 0
To https://teaching.csap.snu.ac.kr/yourname/homework-8.git
   b170706..1efbb18  main -> main
```
