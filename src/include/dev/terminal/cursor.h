#ifndef _cursor_dot_H
#define _cursor_dot_H

#include <stddef.h>
#include <stdbool.h>

struct cursor_info {
	size_t x;
	size_t y;
	bool blink;
};
typedef struct cursor_info cursor_t;

enum cursor_ret {
	SUCCESS,
	MAX_HEIGHT
};

/* Set cursor coorditates to (0,0) */
void cursor_setdefault(cursor_t *terminal_cursor);

/*
 Move cursor to new line
 Return value:
	SUCCESS if max screen height is not reached
	MAX_HEIGHT if max screen height is reached
*/
enum cursor_ret cursor_newline(cursor_t *terminal_cursor);

/*
 Set cursor to next char. If max screen width is reached
 call cursor_newline().
 Return value:
 	Same like cursor_newline()
*/
enum cursor_ret cursor_nextchar(cursor_t *terminal_cursor);

#endif
