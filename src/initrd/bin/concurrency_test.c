#include <libc/syscalls.h>
#include <libc/stdio.h>

void _busy_wait(size_t iter)
{
	size_t i = 0;
	while (i < iter)
		i++;
}

int main()
{
	pid_t p, p2;

	p = fork();
	switch(p) {
	case 0:
		for (size_t i=0; i < 4; i++) {
			printf("process 2\n");
			_busy_wait(3000000);
		}
		break;
	default:
		p2 = fork();
		if (p2 == 0) {
			for (size_t i=0; i < 4; i++) {
				printf("process 3\n");
				_busy_wait(3000000);
			}
		} else {
			for (size_t i=0; i < 4; i++) {
				_busy_wait(3000000);
				printf("process 1\n");
			}
		}
	}

	return 0;
}
