#include <libc/syscalls.h>
#include <libc/stdio.h>

extern int errno;

int main()
{
    char *argv[1];
    pid_t pid;

    pid = fork();

    switch (pid) {
        case 0:
            argv[0] = NULL;
            if (execve("/bin/shell", &argv[0]) < 0) {
                 perror("[ERROR]");
                 printf("Halting system");
            }
            break;
      default:
          waitpid(pid);
    }

    /* After implementing power management */
    // halt system
    return 0;
}