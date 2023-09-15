#ifndef __syscalls_dot_H
#define __syscalls_dot_H

#include <kernel/task.h>
#include <kernel/vfs.h>


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
#define PROT_READ  1
#define PROT_WRITE 2

void *mmap(void *addr, size_t size, int prot, int fd, size_t offset);
void *do_mmap(task_t *t, void *addr, size_t size, 
			int prot, int fd, size_t offset);

int munmap(void *addr);
int do_munmap(task_t *t, void *addr);

int brk(void *addr);
int do_brk(task_t *t, void *addr);

void *sbrk(intptr_t inc);
void *do_sbrk(task_t *t, intptr_t inc);

/* Inter-process comunication */

/* Virtual file system */
#define O_CREAT  1
#define O_RDONLY 2
#define O_WRONLY 4
#define O_RDWR	 8
#define O_DIRECTORY 16

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int open(char *path, int flags);
int do_open(task_t *t, char *path, int flags);

int close(int fd);
int do_close(task_t *t, int fd);

long read(int fd, void *buff, size_t size);
long do_read(task_t *t, int fd, void *buff, size_t size);

long write(int fd, const void *buff, size_t size);
long do_write(task_t *t, int fd, const void *buff, size_t size);

int remove(char *path);
int do_remove(char *path);

int mkdir(char *path, int mode);
int do_mkdir(char *path, int mode);

int rmdir(char *path);
int do_rmdir(char *path);

int readdir(int fd, vfs_dirent_t *dent);
int do_readdir(task_t *t, int fd, vfs_dirent_t *dent);

long lseek(int fd, long offset, int whence);
long do_lseek(task_t *t, int fd, long offset, int whence);
#endif
