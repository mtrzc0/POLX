#ifndef __syscalls_dot_H
#define __syscalls_dot_H

#include <kernel/task.h>


/* Tasks */
typedef tid_t pid_t; 

void exit(int status); // syscall
void do_exit(task_t *t, int status); // In kernel use

pid_t fork(void);
pid_t do_fork(task_t *t);

int execve(const char *path, char *const argv[]);
int do_execve(task_t *t, char *path, char *const argv[]);

pid_t getpid(void);
pid_t getppid(void);

int do_waitpid(task_t *t, pid_t pid);
int waitpid(pid_t pid);

/* Memory */

/* Inter-process comunication */

/* Virtual file system */
#define O_CREAT  1
#define O_RDONLY 2
#define O_WRONLY 4
#define O_RDWR	 8

int open(char *path, int flags);
int do_open(task_t *t, char *path, int flags);

int close(int fd);
int do_close(task_t *t, int fd);

long read(int fd, void *buff, size_t size);
long do_read(task_t *t, int fd, void *buff, size_t size);

long write(int fd, const void *buff, size_t size);
long do_write(task_t *t, int fd, const void *buff, size_t size);
#endif
