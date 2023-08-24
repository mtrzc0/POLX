#include <stddef.h>
#include <stdint.h>
#include <kernel/klib/stdio.h>
#include <dev/terminal/vga_text_mode.h>

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
