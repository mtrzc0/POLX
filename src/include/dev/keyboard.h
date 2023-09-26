#ifndef __keyboard_dot_H
#define __keyboard_dot_H

#define DATA_REG 0x60
#define CTRL_REG 0x64

typedef enum spec_keys {
	ESC = 1,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	BACKSPACE,
	SHIFT,
	CTRL,
	ALT,
	CAPSLOCK,
	TAB,
	HOME,
	END,
	DEL,
	ARR_LEFT,
	ARR_RIGHT,
	ARR_UP,
	ARR_DOWN
} keyboard_layout_special_keys;

void keyboard_isr(void);

#endif
