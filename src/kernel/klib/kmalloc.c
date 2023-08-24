#include <kernel/arch.h>
#include <kernel/klib/stdio.h>
#include <kernel/klib/stdlib.h>
#include <kernel/klib/_internal_kmalloc.h>

struct malloc_state malloc_state;

static chunkptr_t _create_chunk(void *mem_addr, size_t aligned_size)
{
	chunkptr_t chunk = (chunkptr_t)mem_addr;
	
	if (aligned_size < MINIMAL_CHUNK_SIZE)
		chunk->size = MINIMAL_CHUNK_SIZE;
	else
		chunk->size = aligned_size;

	chunk->free = 0;
	chunk->prev_free = 0;

	return chunk;
}

static void __malloc_init_bin(bin_t *bin, size_t stored_size)
{
	bin->stored_size = stored_size;
	bin->counter = 0;
	bin->first = NULL;
	bin->last = NULL;
}

static void _malloc_init(void)
{
	bin_t *bins = &malloc_state.bins[0];

	/* Initialize bins structs */
	for (size_t i=0, size=32; i < SIZEBINS; i++, size+=CHUNK_ALIGNMENT) {
		/* GP sizebins */
		__malloc_init_bin(&bins[i], size);
	}
	
	/* Taskbin */
	//TODO __malloc_init_bin(&bins[5], sizeof(task_t))
	
	/* Sortedbin */
	__malloc_init_bin(&bins[SORTEDBIN_IDX], 0);

	/* Void chunk initialization */
	malloc_state.void_chunk = _create_chunk((void *)VM_KMALLOC_START, 
					(size_t)(VM_KMALLOC_END - VM_KMALLOC_START));
	malloc_state.void_chunk->free = 1;

	malloc_state.is_initialized = 1;
}

static chunkptr_t _pop_from_sizebin(bin_t *bin)
{
	chunkptr_t chunk;
	chunk = bin->first;
	
	if (bin->counter > 1) {
		bin->first = chunk->next;
	} else {
		bin->first = NULL;
	}

	bin->counter -= 1;

	return chunk;
}

static void _earse_from_sortedbin(chunkptr_t chunk)
{
	bin_t *sortedbin;
	chunkptr_t prev_chunk, next_chunk;

	sortedbin = &malloc_state.bins[SORTEDBIN_IDX];
	
	if (sortedbin->counter == 1) {
		prev_chunk = NULL;
		next_chunk = NULL;
	} else {
		prev_chunk = chunk->prev;
		next_chunk = chunk->next;
	}

	if (prev_chunk == NULL)
		sortedbin->first = next_chunk;
	else
		prev_chunk->next = next_chunk;
	
	if (next_chunk == NULL)
		sortedbin->last = prev_chunk;
	else
		next_chunk->prev = prev_chunk;
	
	sortedbin->counter -= 1;
}

static chunkptr_t _split_chunk(chunkptr_t chunk_to_split, size_t new_size)
{
	chunkptr_t chunk_to_free;
	size_t old_size;

	/* If chunk can't be splitted do nothing */
	if ((size_t)chunk_to_split->size - new_size >= MINIMAL_CHUNK_SIZE) {
		old_size = chunk_to_split->size;
		_create_chunk(chunk_to_split, new_size);
		chunk_to_free = _create_chunk((char *)chunk_to_split + new_size,
								old_size - new_size);
		kfree(chunk2mem(chunk_to_free));
	}

	return chunk_to_split;
}

static chunkptr_t _find_free_chunk(size_t sought_size)
{
	bin_t *sizebins = &malloc_state.bins[0];
	bin_t *sortedbin = &malloc_state.bins[SORTEDBIN_IDX];
	chunkptr_t found_chunk;
	chunkptr_t adjacent_chunk;
	chunkptr_t tmp;

	found_chunk = NULL;

	/* Search for free chunk in sizebins */
	for (size_t i=0; i < SIZEBINS; i++) {
		if (sizebins[i].stored_size == sought_size) {
			if (sizebins[i].counter > 0)
				found_chunk = _pop_from_sizebin(&sizebins[i]);
			
			break;
		}
	}

	/* Search for chunk in sortedbin */
	if (found_chunk == NULL && sortedbin->counter > 0) {
		tmp = sortedbin->first;
		while (tmp != NULL) {
			if (tmp->size >= sought_size) {
				found_chunk = tmp;
				_earse_from_sortedbin(found_chunk);
				break;
			}
			tmp = tmp->next;
		}
	}

	/* Split chunk if possible and update header */
	if (found_chunk != NULL) {
		found_chunk = _split_chunk(found_chunk, sought_size);
		adjacent_chunk = (chunkptr_t)((char *)found_chunk + found_chunk->size);
		adjacent_chunk->prev_free = 0;
		adjacent_chunk->prev_size = 0;
		found_chunk->free = 0;
	}

	return found_chunk;
}

static void _void_chunk_update(void *new_addr, size_t new_size)
{
	malloc_state.void_chunk = _create_chunk(new_addr, new_size);
	malloc_state.void_chunk->free = 1;
}

void *kmalloc(size_t size)
{
	chunkptr_t new_chunk, void_chunk;
	size_t aligned_size;
	size_t tmp_size;

	/* Init malloc on first malloc call */
	if (! malloc_state.is_initialized)
		_malloc_init();
	
	new_chunk = NULL;
	void_chunk = malloc_state.void_chunk;

	/* Align size */
	if (size % CHUNK_ALIGNMENT > 0)
		aligned_size = (size - size % CHUNK_ALIGNMENT) + CHUNK_ALIGNMENT;
	else
		aligned_size = size;

	/* For ensure that we can always fit required header info in chunk */
	if (size + CHUNK_DATA_OFFSET > aligned_size)
		aligned_size += CHUNK_ALIGNMENT;

	/* Try to find sufficent freed chunk */
	new_chunk = _find_free_chunk(aligned_size);
	if (new_chunk == NULL) {
		/* Create new chunk */
		if (void_chunk->size >= aligned_size) {
			tmp_size = void_chunk->size;
			new_chunk = _create_chunk(void_chunk, aligned_size);
			_void_chunk_update((char *)void_chunk + aligned_size,
							tmp_size - aligned_size);
		} /*TODO else {
			merge_adjacent_chunks
			find_free_chunk
			create_chunk
		} */
	}

	/* No sufficent memory, call kernel panic */
	if (new_chunk == NULL) {
		panic("[PANIC] kmalloc run out of memory!");
	}

	return chunk2mem(new_chunk);
}

