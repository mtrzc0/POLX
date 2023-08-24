#ifndef __libc_dot_H
#define __libc_dot_H

#include <stddef.h>
#include <stdint.h>

#define stdout 1

typedef uint32_t pid_t;

extern int _start(void);

pid_t fork(void);

long write(int fd, const void *buff, size_t size);

#endif
