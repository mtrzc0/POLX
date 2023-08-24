#include <libc.h>

int main(int argc, char **argv)
{
	pid_t pid;

	pid = fork();

	if (pid == 0) {
			write(stdout, "Child process\n", 15);
			//while (1) ;
	} else {
			write(stdout, "Parent process\n", 16);
			//while (1) ;
	}
}
