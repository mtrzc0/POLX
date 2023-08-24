# ***Only way to communicate with hardware***
Table of contents:   
0. [Tasks syscalls](#tasks-syscalls)     
1. [Memory syscalls](#memory-syscalls)      
2. [IPC syscalls](#ipc-syscalls)      
3. [VFS syscalls](#vfs-syscalls)      

## Tasks syscalls
### exit
**Syscall number: 0**   
```c
void exit(int status);
```
1. Remove task from scheduler queue   
2. Destroy used fds   
3. Inform awaiters that we had finished job   
4. Destroy TCB   
5. Switch to next task   


### fork
**Syscall number: 1**   
```c
pid_t fork(void);
```
1. Copy address space of parent   
    - Set both children and parent pte's to read only, VMM is responsible for copy on write mechanism.   

2. Create new task with copy of parent context   
3. Copy parent signal mask   
4. Copy parent file descriptors   
5. Set proper return value for parent and child task   
6. Push new task to scheduler queue   

### execve
**Syscall number: 2**   
```c
int execve(const char *path, char *const argv[]);
```
1. Create new address space and load _path_ executable   
2. Create process stack   
3. Copy _argv_ content to process stack   
4. Build new argv array on process stack
5. Destroy old address space   
6. Close non default file descriptors   
7. Restore default signal mask   
8. Update task regs   

### getpid, getppid
**Syscall number: 3, 4**   
```c
pid_t getpid(void);
pid_t getppid(void);
```
1. Return appropriate pid

### waitpid
**Syscall number: 5**   
```c
/* Return exit code of awaited process or -1 and set errno */
int waitpid(pid_t pid)
```
1. Add caller TCB to waiting list   
2. Remove caller from scheduler queue   

## Memory syscalls
### mmap
### brk
### sbrk

## IPC syscalls
### kill

## VFS syscalls
### open
**Syscall number: 9**   
```c
#define O_CREAT  1
#define O_RDONLY 2
#define O_WRONLY 4
#define O_RDWR	 8

int open(char *path, int flags);
```

1. Check if file exist   
- If no   
    - if O_CREAT   
        - create file   
        - add it to vfs tree
    - else   
        - return 'file does not exist'    

2. Open
3. Update fd subsystem
4. Return fd number

### close
**Syscall number: 10**   
```c
int close(int fd);
```
1. Check if fd exist
2. Close
3. Update fd subsystem
4. return success

### read
### write
**Syscall number: 12**
```c
long write(int fd, const void *buff, size_t size);
```
1. Check if fd exist
2. Write _size_ bytes from _buff_ into _fd_
3. Update fd position
4. return number of written bytes 

### remove
### mkdir
### rmdir
### readdir