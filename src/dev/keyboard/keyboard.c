#include <dev/keyboard.h>
#include <dev/terminal/terminal.h>
#include <kernel/klib.h>
#include <stdint.h>
#include <stdbool.h>

static uint8_t layout[128] = {\
0, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE, \
TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', CTRL, \
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', SHIFT, '\\', \
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', SHIFT, 0, ALT, ' ', \
CAPSLOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 0, 0, HOME, ARR_UP, \
0, 0, ARR_LEFT, 0, ARR_RIGHT, 0, END, ARR_DOWN, 0, 0, DEL, 0, 0, 0, F11, F12, \
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static uint8_t capital_layout[128] = {\
0, ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', BACKSPACE, \
TAB, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', CTRL, \
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', SHIFT, '|', \
'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', SHIFT, 0, ALT, ' ', \
CAPSLOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 0, 0, HOME, ARR_UP, \
0, 0, ARR_LEFT, 0, ARR_RIGHT, 0, END, ARR_DOWN, 0, 0, DEL, 0, 0, 0, F11, F12, \
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static bool shift_hold  = false;
static bool ctrl_hold   = false;
static bool alt_hold    = false;
static bool capslock_on = false; 

void keyboard_isr(void)
{
	uint8_t ch = inb(DATA_REG);

	if (ch < 128) {
		//kprintf("num: %d\n", ch);
		switch (layout[ch]) {
		case SHIFT:
			shift_hold = true;
			break;
		case CTRL:
			ctrl_hold = true;
			break;
		case ALT:
			alt_hold = true;
			break;
		case CAPSLOCK:
			capslock_on = !capslock_on;
			break;
		default:
			if (shift_hold || capslock_on)
				terminal_putchar_from_keyboard(capital_layout[ch]);
			else
				terminal_putchar_from_keyboard(layout[ch]);
		}
	} else {
		switch (layout[ch-128]) {
		case SHIFT:
			shift_hold = false;
			break;
		case CTRL:
			ctrl_hold = false;
			break;
		case ALT:
			alt_hold = false;
			break;
		}
	}
}
