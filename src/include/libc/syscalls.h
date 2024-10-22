#ifndef __libc_syscalls_dot_H
#define __libc_syscalls_dot_H

#include <stddef.h>
#include <stdint.h>
#include <kernel_limits.h>

#define stdin  0
#define stdout 1

typedef uint32_t pid_t;

extern int _start(void);

void exit(int status);
pid_t fork(void);
int execve(const char *path, char *const argv[]);
pid_t getpid(void);
pid_t getppid(void);
int waitpid(pid_t pid);

#define PROT_READ  1
#define PROT_WRITE 2

void *mmap(void *addr, size_t size, int prot, int fd, size_t offset);
int munmap(void *addr);
int brk(void *addr);
void *sbrk(intptr_t inc);

#define O_CREAT  1
#define O_RDONLY 2
#define O_WRONLY 4
#define O_RDWR	 8
#define O_DIRECTORY 16

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct dirent {
	char name[MAX_FILENAME];
	size_t ino;
};

int open(char *path, int flags);
int close(int fd);
long read(int fd, void *buff, size_t size);
long write(int fd, const void *buff, size_t size);
int remove(char *path);
int mkdir(char *path, int mode);
int rmdir(char *path);
int readdir(int fd, struct dirent *dent);
long lseek(int fd, long offset, int whence);
#endif
