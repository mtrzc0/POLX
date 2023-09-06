#ifndef __libc_dot_H
#define __libc_dot_H

#include <stddef.h>
#include <stdint.h>

#define stdout 1

typedef uint32_t pid_t;

extern int _start(void);

void exit(int status);
pid_t fork(void);
int execve(const char *path, char *const argv[]);
pid_t getpid(void);
pid_t getppid(void);
int waitpid(pid_t pid);


#define O_CREAT  1
#define O_RDONLY 2
#define O_WRONLY 4
#define O_RDWR	 8

int open(char *path, int flags);
int close(int fd);
long write(int fd, const void *buff, size_t size);

#endif
