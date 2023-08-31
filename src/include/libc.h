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


long write(int fd, const void *buff, size_t size);

#endif
