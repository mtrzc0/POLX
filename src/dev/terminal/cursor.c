#include <stddef.h>
#include <stdbool.h>

#include <dev/terminal/cursor.h>
#include <dev/terminal/vga_text_mode.h>

void cursor_setdefault(cursor_t *cursor)
{
	cursor->x = 0;
	cursor->y = 0;
	cursor->line_x = -1;
	cursor->line_y = -1;
	cursor->position_ctr = 0;
	cursor->chars_ctr = 0;
	cursor->blink = true;
	vga_set_cursor(0, 0);
}

enum cursor_ret cursor_newline(cursor_t *cursor)
{
	enum cursor_ret ret;

	if (cursor->y == VGA_HEIGHT) {
		ret = MAX_HEIGHT;
	} else {
		cursor->y += 1;
		ret = SUCCESS;
	}

	cursor->x = 0;
	vga_set_cursor(cursor->x, cursor->y);
	return ret;
}

enum cursor_ret cursor_newline_kbd(cursor_t *cursor)
{
	enum cursor_ret ret;
	size_t rows;

	if (cursor->y == VGA_HEIGHT) {
		ret = MAX_HEIGHT;
	} else {
		rows = (size_t)(cursor->chars_ctr/(VGA_WIDTH));
		cursor->y = cursor->line_y + rows + 1;
		ret = SUCCESS;
	}

	cursor->line_x = 0;
	cursor->line_y = cursor->y;
	
	cursor->position_ctr = 0;
	cursor->chars_ctr = 0;
	cursor->x = 0;
	vga_set_cursor(cursor->x, cursor->y);
	return ret;
}

enum cursor_ret cursor_nextchar(cursor_t *cursor)
{
	enum cursor_ret ret;

	if (cursor->x == VGA_WIDTH-1) {
		ret = cursor_newline(cursor);
	} else {
		cursor->x += 1;
		ret = SUCCESS;
	}

	vga_set_cursor(cursor->x, cursor->y);
	return ret;
}

enum cursor_ret cursor_nextchar_kbd(cursor_t *cursor)
{
	cursor->chars_ctr += 1;
	cursor->position_ctr += 1;
	
	return cursor_nextchar(cursor);
}

void cursor_move_left(cursor_t *cursor, size_t max)
{
	/* Cannot go any further */
	if (cursor->position_ctr == 0)
		return;

	/* HOME button */
	if (max) {
		cursor->x = cursor->line_x;
		cursor->y = cursor->line_y;
		cursor->position_ctr = 0;
	} else {
	/* Left arrow button */
		if (cursor->x == 0 && cursor->y > 0) {
			cursor->x = VGA_WIDTH-1;;
			cursor->y -= 1;
		} else {
			cursor->x -= 1;
		}

		cursor->position_ctr -= 1;
	}
	
	vga_set_cursor(cursor->x, cursor->y);
}

void cursor_move_right(cursor_t *cursor, size_t max)
{
	size_t rows, cols;

	if (cursor->position_ctr == cursor->chars_ctr)
		return;

	/* END button */
	if (max) {
		rows = (size_t)(cursor->chars_ctr/(VGA_WIDTH));
		cols = (size_t)(cursor->chars_ctr % VGA_WIDTH);
		cursor->y = cursor->line_y + rows;
		cursor->x = cursor->line_x + cols;
		cursor->position_ctr = cursor->chars_ctr;
	} else {
	/* Right arrow button */
		if (cursor->x == VGA_WIDTH-1) {
			cursor->x = 0;
			cursor->y += 1;
		} else {
			cursor->x += 1;
		}
	
		cursor->position_ctr += 1;
	}

	vga_set_cursor(cursor->x, cursor->y);
}

void cursor_remove_char(cursor_t *cursor)
{
	if (cursor->chars_ctr > 0)
		cursor->chars_ctr -= 1;
	
	if (cursor->position_ctr > cursor->chars_ctr)
		cursor->position_ctr = cursor->chars_ctr;
}
