#include <kernel/fd.h>

static fd_t *first_fd = NULL;

void fd_add(fd_t *fd)
{
	if (first_fd == NULL) {
		first_fd = fd;
	} else {
		fd->next = first_fd;
		first_fd->prev = fd;
		first_fd = fd;
	}
}

void fd_remove(fd_t *fd)
{
	/* Remove first element in list */
	if (fd->prev == NULL) {
		first_fd = fd->next;
		first_fd->prev = NULL;
		return;
	}

	/* Remove last element */
	if (fd->next == NULL) {
		fd->prev->next = NULL;
		return;
	}

	fd->prev->next = fd->next;
	fd->next->prev = fd->prev;
}
