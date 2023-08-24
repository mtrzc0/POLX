#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/klib/string.h>

#include <dev/terminal/terminal.h>

terminal_t terminal;

static void _terminal_scroll(void)
{
	for (size_t i=0; i < VGA_HEIGHT; i++) {
		vga_move_row_up(terminal.vga_buffer, i+1);
	}
	vga_clear_row(terminal.vga_buffer, VGA_HEIGHT,
				terminal.terminal_color);
}

static void _terminal_putentry(char c)
{
	vga_put_entry(	terminal.vga_buffer,
			terminal.terminal_cursor.x,
			terminal.terminal_cursor.y,
			terminal.terminal_color,
			c );
	
	if (cursor_nextchar(&terminal.terminal_cursor) == MAX_HEIGHT) {
		_terminal_scroll();
	}
}

bool is_terminal_initialized(void)
{
	return terminal.is_initialized;
}

void terminal_clear(void)
{
	for (size_t y=0; y < VGA_HEIGHT; y++) {
		//for (size_t x=0; x < VGA_WIDTH; x++) {
		//	vga_put_entry(terminal->vga_buffer, x, y,
		//			terminal->terminal_color, ' ');
		//}
		vga_clear_row(terminal.vga_buffer, y,
				terminal.terminal_color);
	}

	cursor_setdefault(&terminal.terminal_cursor);
}

void terminal_init(void)
{
	terminal.vga_buffer = (uint16_t *) VGA_BUFFER;
	terminal.terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_clear();
	terminal.is_initialized = true;
}

void terminal_putchar(char c)
{
	if (c == '\n') {
		if (cursor_newline(&terminal.terminal_cursor) == MAX_HEIGHT) {
			_terminal_scroll();
		}
	} else {
		_terminal_putentry(c);
	}
}

void terminal_writestring(const char *string)
{
	size_t len = strlen(string);
	for (size_t i=0; i < len; i++) {
		terminal_putchar(string[i]);
	}
}
