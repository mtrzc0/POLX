#include <stddef.h>
#include <stdint.h>
#include <kernel/klib/stdio.h>
#include <dev/terminal/vga_text_mode.h>
#include <dev/terminal/terminal.h>

static uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

void vga_put_entry(uint16_t *vga_buffer, size_t x, size_t y, uint8_t color, char c)
{
	size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = vga_entry(c, color);
}

void vga_set_cursor(size_t x, size_t y)
{
	uint16_t position = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(position & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

// temporary function (it is task for kernel memory management unit!)
static void _mov_row(uint16_t *src, uint16_t *dst)
{
	for (size_t i=0; i < VGA_WIDTH; i++) {
		dst[i] = src[i];
	}
}

// temporary function (it is task for kernel memory management unit!)
static void _fill_row(uint16_t *src, uint8_t color, char c)
{
	for (size_t i=0; i < VGA_WIDTH; i++) {
		src[i] = vga_entry(c, color);
	}
}

void vga_clear_row(uint16_t *vga_buffer, size_t row, uint8_t color)
{
	uint16_t *src = &vga_buffer[row *VGA_WIDTH];
	_fill_row(src, color, ' ');
}

void vga_move_row_up(uint16_t *vga_buffer, size_t row)
{
	uint16_t *src, *dst;

	if (row > 0) {
		src = &vga_buffer[row * VGA_WIDTH];
		dst = &vga_buffer[(row-1) * VGA_WIDTH];
		_mov_row(src, dst);
	}
}

int vga_copy_line(uint16_t *vga_buffer, size_t start_x,
			size_t start_y, size_t len, char *buffer)
{
	size_t idx;
	uint16_t *src;

	if (start_y > 0) {
		if ((start_y-1) * VGA_WIDTH + start_x + len > TERMINAL_MAX_LINE_LEN)
			return -1;
	} else {
		if (start_x + len > TERMINAL_MAX_LINE_LEN)
			return -1;
	}

	src = &vga_buffer[start_y * VGA_WIDTH + start_x];
	idx = 0;
	while (idx < len) {
		buffer[idx] = (char)(src[idx] & 0xff);
		idx++;
	}

	return 0;
}

int vga_paste_line(uint16_t *vga_buffer, size_t start_x, size_t start_y, 
				 size_t len, char *buffer, uint8_t color)
{
	size_t idx, buffer_idx;
	uint16_t *dst;

	if (start_y > 0) {
		if ((start_y-1) * VGA_WIDTH + start_x + len > TERMINAL_MAX_LINE_LEN)
			return -1;
	} else {
		if (start_x + len > TERMINAL_MAX_LINE_LEN)
			return -1;
	}

	dst = &vga_buffer[start_y * VGA_WIDTH + start_x];
	idx = 0;
	buffer_idx = 0;
	while (buffer_idx < len) {
		if (buffer[buffer_idx] == '\0') {
			buffer_idx++;
			continue;
		}

		dst[idx] = vga_entry(buffer[buffer_idx], color);
		
		buffer_idx++;
		idx++;
	}

	while (idx < buffer_idx) {
		dst[idx] = vga_entry(' ', color);
		idx++;
	}

	return 0;
}
