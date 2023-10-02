#include <libc/syscalls.h>
#include <libc/stdarg.h>
#include <libc/string.h>

static void _add_to_start(char *dest, char c, int last_idx)
{
	/* Move elements forward */
	for (int i=last_idx; i > 0; i--)
		dest[i] = dest[i-1];

	dest[0] = c;
	dest[last_idx+1] = '\0';
}

static size_t _itoa(int n, char *dest, int base, size_t max_len)
{
	int rest, neg;
	size_t idx;
	neg = 0;
	idx = 0;

	if (n == 0) {
		dest[0] = '0';
		return 1;
	}

	if (n < 0) {
		neg = 1;
		n *= -1;
	}

	while (n > 0 && idx < max_len) {
		rest = n % base;
		
		/* Letters from A to F (only if base is 16) */
		if (rest > 9)
			_add_to_start(dest, rest+55, idx);
		/* Digits from 0 to 9 */
		else
			_add_to_start(dest, rest+48, idx);

		n /= base;
		idx++;
	}

	if (neg) {
		_add_to_start(dest, '-', idx);
		idx += 1;
	}

	return idx;
}

int printf(const char *restrict format, ...)
{
	va_list args;
	size_t string_len, tmp;
	char string[TERMINAL_RESOLUTION+1];

	va_start(args, format);
	string_len = 0;
	for (size_t i=0; i < strlen(format) && string_len < TERMINAL_RESOLUTION; i++) {
		if (format[i] == '%') {
			switch(format[i+1]) {
			case '%':
				string[string_len] = '%';
				string_len++;
				break;
			case 'd':
				tmp = TERMINAL_RESOLUTION - string_len;
				string_len += _itoa(va_arg(args, int), &string[string_len], 10, tmp);
				break;
			case 'x':
				tmp = TERMINAL_RESOLUTION - string_len;
				string_len += _itoa(va_arg(args, int), &string[string_len], 16, tmp);
				break;
			case 's':
				// TMP solution
				char *str = va_arg(args, char *);
				for (size_t i=0; i < strlen(str) && string_len < TERMINAL_RESOLUTION; i++) {
					string[string_len] = str[i];
					string_len++;
				}
				break;
			}
			i++;
			continue;
		}

		string[string_len] = format[i];
		string_len++;
	}
	va_end(args);
	
	write(stdout, string, string_len);

	return string_len;
}

