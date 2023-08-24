#include <stddef.h>
#include <stdbool.h>

#include <dev/terminal/cursor.h>
#include <dev/terminal/vga_text_mode.h>

void cursor_setdefault(cursor_t *terminal_cursor)
{
	terminal_cursor->x = 0;
	terminal_cursor->y = 0;
	terminal_cursor->blink = true;
	vga_set_cursor(0, 0);
}

enum cursor_ret cursor_newline(cursor_t *terminal_cursor)
{
	enum cursor_ret ret;

	if (terminal_cursor->y == VGA_HEIGHT) {
		ret = MAX_HEIGHT;
	} else {
		terminal_cursor->y += 1;
		ret = SUCCESS;
	}

	terminal_cursor->x = 0;
	vga_set_cursor(terminal_cursor->x, terminal_cursor->y);
	return ret;
}

enum cursor_ret cursor_nextchar(cursor_t *terminal_cursor)
{
	enum cursor_ret ret;

	if (terminal_cursor->x == VGA_WIDTH) {
		ret = cursor_newline(terminal_cursor);
	} else {
		ret = SUCCESS;
	}

	terminal_cursor->x += 1;
	vga_set_cursor(terminal_cursor->x, terminal_cursor->y);
	return ret;
}
