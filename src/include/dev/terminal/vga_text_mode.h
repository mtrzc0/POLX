#ifndef _vga_text_mode_dot_H
#define _vga_text_mode_dot_H

#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 24
#define VGA_BUFFER 0xc00b8000

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15
};

/*
 Calculate entry color from arguments
 Return value:
 	8 bit vga color
*/
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

/* Build correct vga entry and put it on screen */
void vga_put_entry(uint16_t *vga_buffer, size_t x, size_t y, uint8_t color, char c);

/* Set entry cursor to (x, y) position */
void vga_set_cursor(size_t x, size_t y);

/* Fill with spaces specified row */
void vga_clear_row(uint16_t *vga_buffer, size_t row, uint8_t color);

/* Move specified row to previous row */
void vga_move_row_up(uint16_t *vga_buffer, size_t row);
#endif
