#include <libc/syscalls.h>
#include <libc/stdio.h>
#include <errno.h>

#define CHUNK_SIZE 1024

void _print_ussage(char *filename)
{
	printf("Ussage: %s [FILE]\n", filename);
	exit(0);
}

int main(int argc, char **argv)
{
	int fd;
	long read_len;
	char buff[CHUNK_SIZE+1];

	if (argc < 3)
		_print_ussage(argv[0]);

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("%s: cannot access %s", argv[0], argv[1]);
		perror("");
		exit(errno);
	}

	read_len = read(fd, &buff[0], CHUNK_SIZE);
	while (read_len == CHUNK_SIZE) {
		buff[CHUNK_SIZE] = '\0';
		printf("%s", buff);
		read_len = read(fd, &buff[0], CHUNK_SIZE);
	}
	
	buff[read_len] = '\0';
	printf("%s", buff);

	close(fd);
	return 0;
}
