# Homework 7 - Processes

In this homework, you learn how to
*  create child processes
*  catch signals
*  wait for children processes and evaluate the return status
*  execute other programs

[[_TOC_]]



## Before you begin

Do not forget to fork this project into your namespace and verify that its visibility is set to *private*.



## Task 1

### Description

Write a program that creates a child process. The parent process then calls a function `parent()` which prints `"[<pid>] Hello from parent."`
The child process calls a function called `child()` which prints `"[<pid>] Hello from child."`  
`<pid>` represents the PID of the process.

### Logistics

Copy the template `template.c` to file `task1.c`, then edit `task1.c` and implement your solution for task 1.
```bash
$ cp template.c task1.c
$ vi task1.c
```

Use GNU Make to compile task 1.
```bash
$ make task1
$ ./task1
[7375] Hello from parent.
[7376] Hello from child.
```

Make sure to add the file to your repository and commit your code frequently.
```bash
$ git add task1.c
$ git commit -m "Task 1: create child progress"
```



## Task 2

### Logistics

Copy your solution from task 1, `task1.c` to file `task2.c`, then edit `task2.c` and implement your solution for task 2.
```bash
$ cp task1.c task2.c
$ vi task2.c
```

Use GNU Make to compile task 2.
```bash
$ make task2
$ ./task2
...
```

Make sure to add the file to your repository and commit your code frequently.
```bash
$ git add task2.c
$ git commit -m "Task 2: install signal handlers."
```

### Description

Modify the program from task 1 as follows:
  * in the child process
    * install two signal handlers, for example `hdl_sigusr1()` and `hdl_sigusr2()`, for SIGUSR1 and SIGUSR2 in the `child()` function.
    * the handler for SIGUSR1 increments a global counter by 1 for every invokation of the handler.
    * the handler for SIGUSR2 exits the child process and sets the return code to the value of the global counter.
  * in the parent process
    * wait for the child to end (you may want to pass the child's pid to the `parent()` function).
    * print whether the child exited normally (WIFEXITED) and the number of SIGUSR1 signals the child has received.

Feel free to add extra `printf()` statements to understand what happens in your program.

Run your program from a terminal:
```bash
$ make task2
$ ./task2
[7375] Hello from parent.
[7375]   Waiting for child to terminate...
[7376] Hello from child.
[7376]   SIGUSR1 handler installed.
[7376]   SIGUSR2 handler installed.
[7376]   Waiting for signals...
```

Open another terminal and use the `kill` command to send SIGUSR1 and SIGUSR2 signals to the child process (note the child's PID in the printout)
```bash
<this terminal>            |       <terminal running parent+child>
$ kill -s USR1 7376        |       [7376] Child received SIGUSR1! Count = 1.
$ kill -s USR1 7376        |       [7376] Child received SIGUSR1! Count = 2.
$ kill -s USR1 7376        |       [7376] Child received SIGUSR1! Count = 3.
$ kill -s USR2 7376        |       [7375] Child has terminated normally. It has received 3 SIGUSR1 signals.
```



## Task 3

### Logistics

Copy the template `template.c` to file `task3.c`, then edit `task3.c` and implement your solution for task 3.
```bash
$ cp template.c task3.c
$ vi task3.c
```

Use GNU Make to compile task 1.
```bash
$ make task3
$ ./task3
...
```

Make sure to add the file to your repository and commit your code frequently.
```bash
$ git add task3.c
$ git commit -m "Task 3: execute other process"
```


### Description

The program in task3 executes another program using the `execvp()` function. Task3 accepts the program name and eventual command line arguments on the command line as follows:
```bash
$ ./task3 ls                   # execute program 'ls' with no additional command line arguments.
                               #   The result should be equivalent to typing 'ls' at the terminal.
$ ./task3 ls -l                # execute program 'ls' with the command line argument '-l'.
                               #   The result should be equivalent to typing 'ls -l' at the terminal.
$ ./task3 cat task3.c          # execute 'cat' program with command line argument 'task3.c'.
                               #   This should be equivalent to typing 'cat task3.c' at the terminal.
```
Your program should print proper error messages using `perror()` when something goes wrong:
```bash
$ ./task3 LS
execv() failed: No such file or directory
```

Feel free to add extra `printf()` statements to understand what happens in your program.



## Submission

If you have committed your work as instructed at the end of tasks 1-3, you can submit your solution by pushing your commits to the Gitlab server:
```bash
$ git push
Counting objects: 4, done.
Delta compression using up to 4 threads.
Compressing objects: 100% (3/3), done.
Writing objects: 100% (4/4), 379 bytes | 0 bytes/s, done.
Total 4 (delta 2), reused 0 (delta 0)
To https://teaching.csap.snu.ac.kr/yourname/homework-7.git
   ec4fa12..ce69fec  main -> main 
```
