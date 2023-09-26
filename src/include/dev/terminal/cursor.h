#ifndef _cursor_dot_H
#define _cursor_dot_H

#include <stddef.h>
#include <stdbool.h>

struct cursor_info {
	size_t x;
	size_t y;
	bool blink;

	
	/* Coordinates of the beggining of the input line */
	size_t line_x;
	size_t line_y;
	
	/* Keep track of cursor movement in input line */
	size_t position_ctr;
	/* Counter of printed chars in input line */
	size_t chars_ctr;
	
};
typedef struct cursor_info cursor_t;

enum cursor_ret {
	SUCCESS,
	MAX_HEIGHT
};

/* Set cursor coorditates to (0,0) */
void cursor_setdefault(cursor_t *cursor);

/*
 Move cursor to new line
 Return value:
	SUCCESS if max screen height is not reached
	MAX_HEIGHT if max screen height is reached
*/
enum cursor_ret cursor_newline(cursor_t *cursor);

enum cursor_ret cursor_newline_kbd(cursor_t *cursor);

/*
 Set cursor to next char. If max screen width is reached
 call cursor_newline().
 Return value:
 	Same like cursor_newline()
*/
enum cursor_ret cursor_nextchar(cursor_t *cursor);

enum cursor_ret cursor_nextchar_kbd(cursor_t *cursor);

/* Move cursor to side one time or to max position if max set */
void cursor_move_left(cursor_t *cursor, size_t max);
void cursor_move_right(cursor_t *cursor, size_t max);

/* Update cursor inside data */
void cursor_remove_char(cursor_t *cursor);
#endif
