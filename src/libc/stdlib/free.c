#include <libc/_internal_malloc.h>

extern struct malloc_state malloc_state;

static bin_t *_find_suitable_bin(size_t size)
{
	bin_t *bins, *found_bin;

	bins = &malloc_state.bins[0];
	found_bin = NULL;

	for (size_t i=0; i < SIZEBINS; i++) {
		if (bins[i].stored_size == size) {
			found_bin = &bins[i];
			break;
		}
	}

	return found_bin;
}

static void _push_to_sizebin(bin_t *sizebin, chunkptr_t chunk)
{
	chunk->next = sizebin->first;
	sizebin->first = chunk;

	sizebin->counter += 1;
}

static void _insert_in_sortedbin(chunkptr_t chunk)
{
	bin_t *sortedbin;
	chunkptr_t tmp;

	sortedbin = &malloc_state.bins[SORTEDBIN_IDX];
	tmp = sortedbin->first;

	switch(sortedbin->counter) {
	case 0: // Empty sortedbin
		sortedbin->first = chunk;
		sortedbin->last = chunk;
		chunk->next = NULL;
		chunk->prev = NULL;
		break;
	case 1:
		if (tmp->size > chunk->size) {
			sortedbin->first = chunk;
			chunk->next = tmp;
			chunk->prev = NULL;
			tmp->prev = chunk;
		} else {
			sortedbin->last = chunk;
			chunk->next = NULL;
			chunk->prev = tmp;
			tmp->next = chunk;
		}
		break;
	default: // Find right place
		/* Chunk has the smallest size */
		if (tmp->size > chunk->size) {
			sortedbin->first = chunk;
			chunk->next = tmp;
			chunk->prev = NULL;
			tmp->prev = chunk;
			tmp = NULL;
		} else if (sortedbin->last->size <= chunk->size) {
		/* Chunk has the biggest size */
			tmp = sortedbin->last;
			sortedbin->last = chunk;
			chunk->next = NULL;
			chunk->prev = tmp;
			tmp->next = chunk;
			tmp = NULL;
		}

		/* Find chunk place inside llist */
		while (tmp != NULL) {
			if(tmp->next != NULL) {
				if (tmp->size <= chunk->size && tmp->next->size >= chunk->size) {
					chunk->prev = tmp;
					chunk->next = tmp->next;
					tmp->next->prev = chunk;
					tmp->next = chunk;
					break;
				}
			} else {
				/* Last chunk was occured */
				break;
			}
			
			tmp = tmp->next;
		}
	}

	sortedbin->counter += 1;
}

void free(void *ptr)
{
	chunkptr_t chunk, next_chunk;
	bin_t *dest_bin;

	if (ptr == NULL)
		return;

	chunk = mem2chunk(ptr);

	/* Security check */
	if (! malloc_state.is_initialized)
		return;
	if (chunk->free)
		return;

	/* Insert chunk into the suitable bin */
	dest_bin = _find_suitable_bin((size_t)chunk->size);
	if (dest_bin == NULL) {
		_insert_in_sortedbin(chunk);
	} else {
		_push_to_sizebin(dest_bin, chunk);
	}

	/* Update adjacent chunk header */
	next_chunk = (chunkptr_t) ((char *)chunk + chunk->size);
	/* Check if adjacent chunk exist */
	if (malloc_state.void_chunk > next_chunk) {
		next_chunk->prev_free = 1;
		next_chunk->prev_size = (size_t)chunk->size;
	}

	chunk->free = 1;
}
