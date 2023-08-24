#ifndef _terminal_dot_H
#define _terminal_dot_H

#include <stdint.h>
#include <stdbool.h>
#include "vga_text_mode.h"
#include "cursor.h"

struct terminal_info {
	bool is_initialized;
	uint16_t *vga_buffer;
	uint8_t terminal_color;
	cursor_t terminal_cursor;
};
typedef struct terminal_info terminal_t;

/* Return 0 if not or 1 if yes */
bool is_terminal_initialized(void);

/* Fill terminal with spaces and set default cursor position */
void terminal_clear(void);

/* Initialize terminal object with default values and clear terminal */
void terminal_init(void);

/* Put char on screen with newline char respect */
void terminal_putchar(char c);

/* Put string ended with \0 on screen */
void terminal_writestring(const char *string);

#endif
