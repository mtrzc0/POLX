#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/devfs.h>
#include <kernel/klib.h>

#include <dev/terminal/terminal.h>
#include <dev/keyboard.h>

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
}

static void _terminal_remove_from_inputline(size_t idx)
{
	size_t row, col;
	cursor_t *cursor;

	cursor =  &terminal.terminal_cursor;
		

	row = cursor->line_y;
	col = cursor->line_x;

	if (vga_copy_line(terminal.vga_buffer, col, row,
	    cursor->chars_ctr, terminal.stdin_line) < 0) {
		panic("[BUG] Terminal tried to copy line outside of vga buffer!\n");
	}

	terminal.stdin_len = cursor->chars_ctr;
	terminal.stdin_line[idx] = '\0';
	if (vga_paste_line(terminal.vga_buffer, col, row, terminal.stdin_len, 
			  terminal.stdin_line, terminal.terminal_color) < 0) {
		panic("[BUG] Terminal tried to paste line outside of vga buffer!\n");
	}
}

static void _terminal_insert_into_inputline(char c, size_t idx)
{
	size_t row, col, len;
	cursor_t *cursor;

	cursor = &terminal.terminal_cursor;
	
	row = cursor->line_y;
	col = cursor->line_x;

	if (vga_copy_line(terminal.vga_buffer, col, row,
	    cursor->chars_ctr, terminal.stdin_line) < 0) {
		panic("[BUG] Terminal tried to copy line outside of vga buffer!\n");
	}

	_terminal_putentry(c);

	len = cursor->chars_ctr - cursor->position_ctr;

	/* Paste rest of inputline */
	if (vga_paste_line(terminal.vga_buffer, cursor->position_ctr+col+1, row, len,
			  &terminal.stdin_line[idx], terminal.terminal_color) < 0) {
		panic("[BUG] Terminal tried to paste line outside of vga buffer!\n");
	}

	terminal.stdin_len = cursor->chars_ctr + 1;
}

bool is_terminal_initialized(void)
{
	return terminal.is_initialized;
}

void terminal_clear(void)
{
	for (size_t y=0; y < VGA_HEIGHT; y++) {
		vga_clear_row(terminal.vga_buffer, y,
				terminal.terminal_color);
	}

	cursor_setdefault(&terminal.terminal_cursor);
}

void terminal_init(void)
{
	terminal.vga_buffer = (uint16_t *) VGA_BUFFER;
	terminal.terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal.stdin_len = 0;
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
	
		if (cursor_nextchar(&terminal.terminal_cursor) == MAX_HEIGHT)
			_terminal_scroll();
	}

	/* Not entry from keyboard */
	terminal.terminal_cursor.line_x = -1;
	terminal.terminal_cursor.line_y = -1;
}

void terminal_putchar_from_keyboard(char c)
{
	cursor_t *cursor;

	cursor =  &terminal.terminal_cursor;
	
	/* Initialize stdin line coordinates with first entered char */
	if (cursor->line_y == (size_t)-1 && cursor->line_x == (size_t)-1) {
		cursor->line_x = cursor->x;
		cursor->line_y = cursor->y;
	}

	/* ASCII char to print */
	if (c >= 0x20 && c <= 0x7e) {
		if (cursor->position_ctr < cursor->chars_ctr)
			_terminal_insert_into_inputline(c, cursor->position_ctr);
		else
			_terminal_putentry(c);
		
		if (cursor_nextchar_kbd(cursor) == MAX_HEIGHT)
			_terminal_scroll();

		return;
	}

	/* Cursor control or input line edit */
	switch (c) {
	case ARR_LEFT:
		cursor_move_left(cursor, 0);
		break;
	case HOME:
		cursor_move_left(cursor, 1);
		break;
	case ARR_RIGHT:
		cursor_move_right(cursor, 0);
		break;
	case END:
		cursor_move_right(cursor, 1);
		break;
	case '\n':
		if (vga_copy_line(terminal.vga_buffer, cursor->line_x, cursor->line_y,
		    				cursor->chars_ctr, terminal.stdin_line) < 0) {
			panic("[BUG] Terminal tried to copy line outside of vga buffer!\n");
		}
		terminal.stdin_len = cursor->chars_ctr;

		if (cursor_newline_kbd(cursor) == MAX_HEIGHT)
			_terminal_scroll();
	
		dev_stdin_recall(terminal.stdin_line, terminal.stdin_len);
		break;
	case BACKSPACE:
		if (cursor->chars_ctr == 0 || cursor->position_ctr == 0)
			break;

		_terminal_remove_from_inputline(cursor->position_ctr-1);

		cursor_move_left(cursor, 0);
		cursor_remove_char(cursor);
			
		break;
	case DEL:
		if (cursor->chars_ctr == 0)
			break;
		if (cursor->chars_ctr == cursor->position_ctr)
			break;

		_terminal_remove_from_inputline(cursor->position_ctr);
		
		cursor_remove_char(cursor);

		break;
	default:
		break;
	}
}

void terminal_writestring(const char *string)
{
	size_t len = strlen(string);
	for (size_t i=0; i < len; i++) {
		terminal_putchar(string[i]);
	}
}
