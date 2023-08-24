#include <stdint.h>
#include <stddef.h>
#include <kernel/klib/stdarg.h>
#include <kernel/klib/string.h>
#include <dev/terminal/terminal.h>

static void _add_to_start(char *dest, char c, int last_idx)
{
	/* Move elements forward */
	for (int i=last_idx; i > 0; i--)
		dest[i] = dest[i-1];

	dest[0] = c;
	dest[last_idx+1] = '\0';
}

static void _itoa(int n, char *dest, int base)
{
	int rest, idx, neg;
	neg = 0;
	idx = 0;

	if (n == 0)
		_add_to_start(dest, '0', idx);
	
	if (n < 0) {
		neg = 1;
		n *= -1;
	}

	while (n > 0) {
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

	if (neg)
		_add_to_start(dest, '-', idx);
}

void kprintf(const char *fmt, ...)
{
	va_list args;
	char tmp[20];

	/* Initialize terminal if necessary */
	if (!is_terminal_initialized())
		terminal_init();

	va_start(args, fmt);
	for (size_t i=0; i < strlen(fmt); i++) {
		if (fmt[i] == '%') {
			switch(fmt[i+1]) {
			case '%':
				terminal_putchar('%');
				break;
			case 'd':
				_itoa(va_arg(args, int), tmp, 10);
				terminal_writestring(tmp);
				break;
			case 'x':
				_itoa(va_arg(args, int), tmp, 16);
				terminal_writestring(tmp);
				break;
			case 's':
				terminal_writestring(va_arg(args, char *));
				break;
			}
			i++;
			continue;
		}
		terminal_putchar(fmt[i]);
	}
	va_end(args);
}
