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
**Syscall number: 6**   
```c
#define PROT_READ  1
#define PROT_WRITE 2

void *mmap(void *addr, size_t size, int prot, int fd, size_t offset);
```
1. Check if fd is valid and it points to a file
2. Convert prot into flags understood by the VMM subsystem
3. Call proper VMM function
4. return address at fd was mapped or NULL if VMM failed to map file

### munmap
**Syscall number: 7**   
```c
int munmap(void *addr);
```
1. Call proper VMM function
2. return result

**Warning: It works different then its linux counterpart, because it unmapping whole region at _addr_ and there is no posibility to unmap only the part of region.**   

### brk
**Syscall number: 8**   
```c
int brk(void *addr);
```
1. Call proper VMM function

### sbrk
**Syscall number: 9**   
```c
void *sbrk(intptr_t inc);
```
1. Increment actual data segment address by _inc_   
2. Call proper VMM function   

## IPC syscalls
### kill

## VFS syscalls
### open
**Syscall number: 10**   
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
**Syscall number: 11**   
```c
int close(int fd);
```
1. Check if fd exist
2. Close
3. Update fd subsystem
4. return success

### read
**Syscall number: 12**   
```c
long read(int fd, void *buff, size_t size);
```
1. Check if fd exist
2. Read _size_ bytes from _fd_ and write them into _buff_
3. Update fd position
4. return number of read bytes

### write
**Syscall number: 13**
```c
long write(int fd, const void *buff, size_t size);
```
1. Check if fd exist
2. Write _size_ bytes from _buff_ into _fd_
3. Update fd position
4. return number of written bytes 

### remove
**Syscall number: 14**   
```c
int remove(char *path);
```
1. Check if file exist and its not opened by other task
2. Send info to driver about removing file
3. return success or -1 and set errno

### mkdir
**Syscall number: 15**
```c
int mkdir(char *path, int mode);
```
1. Check if directory exist and path is correct
2. Send info to driver to create directory
3. return success or -1 and set errno

### rmdir
**Syscall number: 16**   
```c
int rmdir(char *path);
```
1. Check if directory exist and it is not a mount point
2. Check if directory is empty
3. Send info to driver to remove directory
4. return success or -1 and set errno

### readdir
**Syscall number: 17**   
```c
int readdir(int fd, dirent_t *dent);
```
1. Check if fd exist and it points to directory
2. Get dir entry indicated by file descriptor position
3. Write created dir entry into dent
4. return 1 if success, 0 if end of directory or -1 and set errno

### fseek