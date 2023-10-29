#include <libc/syscalls.h>
#include <libc/string.h>
#include <libc/stdlib.h>
#include <libc/stdio.h>

#include <kernel_limits.h>

#define PROMPT "# "

void _print_help(void)
{
	printf("Basic user shell for TrytonOS:\n");
	printf("type '?' to print this message\n");
	printf("type '$' to get exit code of previous program\n");
}

void _parse_input(char *input, char **argv)
{
	size_t start, argv_idx;
	long i, input_len;
	int is_string;

	/* Count null terminator */
	input_len = strlen(input) + 1;
	
	i = 0;
	start = 0;
	argv_idx = 0;
	is_string = 0;
	while (i < input_len && argv_idx < MAX_ARGV-1) {
		if (input[i] == '"' || is_string == 1) {
			if (is_string == 0) {
				is_string = 1;
				start++;
			} else if (input[i] == '"') {
				is_string = 0;
				input[i] = ' ';	
			}
			i++;
			continue;
		}

		if (input[i] == ' ') {
			input[i] = '\0';
			argv[argv_idx] = &input[start];
			i++;
			argv_idx++;
			start = i;
			continue;
		} else if (input[i] == '\0') {
			argv[argv_idx] = &input[start];
			argv_idx++;
			break;
		}
		
		i++;
	}

	argv[argv_idx] = NULL;
}

int _execute_program(char **argv)
{
	pid_t p;
	int ret;

	p = fork();
	switch (p) {
	case 0:
		if (execve(argv[0], &argv[0]) < 0) {
			printf("Failed to execute %s", argv[0]);
			perror("");
			exit(1);
		}
		break;
	default:
		ret = waitpid(p);
	}

	return ret;
}

int main()
{
	char *argv[MAX_ARGV];
	char *input_line;
	int child_ret;

	child_ret = 0;
	while (1) {
		input_line = readline(PROMPT);
		if (input_line == NULL) {
			perror("");
			continue;
		}
		
		/* Shell commands */
		switch (input_line[0]) {
		case '\0':
			continue;
		case '$':
			printf("Exit code: %d\n", child_ret);
			continue;
		case '?':
			_print_help();
			continue;
		}

		_parse_input(input_line, argv);

		child_ret = _execute_program(argv);
		switch(child_ret) {
		case 139:
			printf("Segmentation fault\n");
			break;
		default:
		}

		free(input_line);
	}
}
