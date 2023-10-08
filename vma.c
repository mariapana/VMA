#include "vma.h"

// ARENA FUNCTIONS

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = malloc(sizeof(arena_t));
	DIE(!arena, "alloc_arena(): malloc() failed\n");

	arena->alloc_list = ll_create(sizeof(block_t));
	DIE(!arena->alloc_list, "alloc_arena(): malloc() failed\n");

	arena->arena_size = size;

	return arena;
}

void dealloc_arena(arena_t *arena)
{
	list_t *b_list = arena->alloc_list;

	if (b_list->head) {
		node_t *b_iterator = b_list->head;

		while (b_iterator) {
			block_t *block = b_iterator->data;
			list_t *m_list = block->miniblock_list;
			node_t *node = m_list->head;

			for (uint64_t i = 0; i < m_list->size; i++) {
				miniblock_t *m = node->data;
				if (m->rw_buffer)
					free(m->rw_buffer);
				node = node->next;
			}

			ll_free(&m_list);
			b_iterator = b_iterator->next;
		}
	}

	ll_free(&b_list);
	free(arena);
}

// (MINI)BLOCK FUNCTIONS

miniblock_t *create_miniblock(const uint64_t address, const uint64_t size)
{
	miniblock_t *miniblock = malloc(sizeof(miniblock_t));
	DIE(!miniblock, "create_miniblock(): malloc() failed\n");

	miniblock->start_address = address;
	miniblock->size = size;
	miniblock->perm = 6;
	miniblock->r = 1;
	miniblock->w = 1;
	miniblock->x = 0;
	miniblock->rw_buffer = NULL;

	return miniblock;
}

block_t *create_block(const uint64_t address, const uint64_t size)
{
	block_t *block = malloc(sizeof(block_t));
	DIE(!block, "create_block(): malloc() failed\n");

	block->size = size;
	block->start_address = address;

	block->miniblock_list = ll_create(sizeof(miniblock_t));
	DIE(!block->miniblock_list, "create_block(): malloc() failed\n");

	return block;
}

/*
*	CASE: The new block is adjacent to the previous and to the next
*		  block
*
*	There are 3 blocks to merge: the previous block (b1),
*								 the new, current block (b2),
*								 the next block (b3)
*	=> b1 will become a larger block with its miniblocks, b2 and b3 as
*	   its miniblock list elements
*/
void double_adjacent(arena_t *arena, uint64_t address, uint64_t size,
					 uint64_t sh_start)
{
	// Determine the previous (b1) and the next (b3) block
	uint64_t index_b1 = sh_start;
	node_t *node_b1 = ll_get_nth_node(arena->alloc_list, index_b1);
	block_t *b1 = node_b1->data;
	block_t *b3 = node_b1->next->data;

	// Create the miniblock equivalent of b2
	miniblock_t *m2 = create_miniblock(address, size);
	// Add m2 to the b1 miniblock list
	list_t *m_list = b1->miniblock_list;
	uint64_t num_of_minib = m_list->size;
	ll_add_nth_node(m_list, num_of_minib, m2);
	free(m2);

	// Add b3 miniblocks to the b1 miniblock list
	list_t *m3_list = b3->miniblock_list;
	node_t *m3_iter = m3_list->head;
	for (uint64_t i = 0; i < m3_list->size; i++) {
		miniblock_t *m3_elem = m3_iter->data;
		ll_add_nth_node(m_list, num_of_minib + 1 + i, m3_elem);
		m3_iter = m3_iter->next;
	}

	// Update b1 size
	uint64_t b2_size = size;
	b1->size += b2_size + b3->size;

	// Free b3 and its miniblock list
	list_t *b3_m_list = b3->miniblock_list;
	for (uint64_t i = 0; i < b3_m_list->size; i++) {
		node_t *node = ll_get_nth_node(b3_m_list, i);
		miniblock_t *m = node->data;
		free(m->rw_buffer);
	}

	ll_free(&b3_m_list);

	node_t *rem_node = ll_remove_nth_node(arena->alloc_list, index_b1 + 1);
	free(rem_node->data);
	free(rem_node);
}

/*
*	CASE: The new block is adjacent to the previous block
*
*	There are 2 blocks to merge: the previous block (b1),
*								 the new, current block (b2)
*	=> b1 will become a larger block with its miniblocks and b2
*/
void left_adjacent(arena_t *arena, uint64_t address, uint64_t size,
				   uint64_t sh_start)
{
	// Determine the previous block (b1)
	uint64_t index_b1 = sh_start;
	node_t *node_b1 = ll_get_nth_node(arena->alloc_list, index_b1);
	block_t *b1 = node_b1->data;

	// Create the miniblock equivalent of b2
	miniblock_t *m2 = create_miniblock(address, size);

	// Add m2 to the end of b1
	list_t *m_list = b1->miniblock_list;
	uint64_t num_of_minib = m_list->size;
	ll_add_nth_node(m_list, num_of_minib, m2);
	free(m2);

	// Update b1 size
	b1->size += size;
}

/*
*	CASE: The new block is adjacent to the next block
*
*	There are 2 blocks to merge: the new, current block (b2),
								 the next block (b3)
*	=> b3 will become a larger block with its miniblocks and b2
*/
void right_adjacent(arena_t *arena, uint64_t address, uint64_t size,
					uint64_t sh_end)
{
	// Determine the previous block (b1)
	uint64_t index_b3 = sh_end;
	node_t *node_b3 = ll_get_nth_node(arena->alloc_list, index_b3);
	block_t *b3 = node_b3->data;

	// Create the miniblock equivalent of b2
	miniblock_t *m2 = create_miniblock(address, size);

	// Add m2 to the beginning of b1
	list_t *m_list = b3->miniblock_list;
	ll_add_nth_node(m_list, 0, m2);
	free(m2);

	// Update b3 size and start address
	b3->start_address = address;
	b3->size += size;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return;
	}

	if (address + size > arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return;
	}

	list_t *b_list = arena->alloc_list;

	// There is no allocated block
	if (!b_list->head) {
		block_t *block = create_block(address, size);
		ll_add_nth_node(b_list, 0, block);

		miniblock_t *miniblock = create_miniblock(address, size);
		ll_add_nth_node(block->miniblock_list, 0, miniblock);

		free(block);
		free(miniblock);

		return;
	}

	if (!zone_is_free(arena, address, size)) {
		printf("This zone was already allocated.\n");
		return;
	}

	// Are there any adjacent blocks?
	int64_t sh_start = share_start(b_list, address);
	int64_t sh_end = share_end(b_list, address + size);

	if (sh_start != -1 && sh_end != -1) {
		double_adjacent(arena, address, size, sh_start);
		return;
	}

	if (sh_start != -1) {
		left_adjacent(arena, address, size, sh_start);
		return;
	}

	if (sh_end != -1) {
		right_adjacent(arena, address, size, sh_end);
		return;
	}

	// No adjacent block
	uint64_t index_b = index_last_block_from_address(arena, address) + 1;
	block_t *block = create_block(address, size);
	ll_add_nth_node(b_list, index_b, block);

	miniblock_t *miniblock = create_miniblock(address, size);
	ll_add_nth_node(block->miniblock_list, 0, miniblock);

	free(block);
	free(miniblock);
}

void free_block(arena_t *arena, const uint64_t address)
{
	miniblock_t *miniblock = miniblock_at_address(arena, address);
	block_t *block = block_containing_address(arena, address);
	if (!miniblock || !block) {
		printf("Invalid address for free.\n");
		return;
	}

	list_t *m_list = block->miniblock_list;
	uint64_t b_index = index_block(arena, address);

	// CASE: The only miniblock -> also free the main block
	if (m_list->size == 1) {
		free(miniblock->rw_buffer);
		ll_free(&m_list);
		node_t *rem_node = ll_remove_nth_node(arena->alloc_list, b_index);
		free(rem_node->data);
		free(rem_node);
	}

	int64_t m_index = index_miniblock(arena, address);
	if (m_index != -1) {
		if (m_index == 0 || m_index == (m_list->size - 1)) {
			// CASE: Start/end -> just remove from m_list
			block->start_address = miniblock->start_address + miniblock->size;
			free(miniblock->rw_buffer);
			node_t *rem_node = ll_remove_nth_node(m_list, m_index);
			free(rem_node->data);
			free(rem_node);
		} else {
			/*
			*	CASE: Middle (Interior) -> separate the block into 2 smaller
			*		  new blocks (b1, b2)
			*	Create 2 new blocks b1 and b2. Populate both blocks' miniblock
			*	lists with the correspondent miniblocks.
			*	Free old blocks and other resources.
			*	Add the 2 new blocks to the arena in the old block's place.
			*/

			uint64_t size_b1 = miniblock_size_sum(m_list, 0, m_index - 1);
			block_t *b1 = create_block(block->start_address, size_b1);
			list_t *m1_list = b1->miniblock_list;
			uint64_t m1_num = miniblock_num(m_list, 0, m_index - 1);
			for (uint64_t i = 0; i < m1_num; i++) {
				node_t *node = ll_get_nth_node(m_list, i);
				miniblock_t *m = node->data;
				ll_add_nth_node(m1_list, i, m);
			}

			uint64_t size_b2 = miniblock_size_sum(m_list, m_index + 1,
												  m_list->size - 1);
			node_t *rem_m_node = ll_get_nth_node(m_list, m_index);
			miniblock_t *rem_m = rem_m_node->data;
			uint64_t addr_b2 = block->start_address + size_b1 + rem_m->size;
			block_t *b2 = create_block(addr_b2, size_b2);
			list_t *m2_list = b2->miniblock_list;
			uint64_t m2_num = miniblock_num(m_list, m_index + 1, m_list->size);
			for (uint64_t i = 0; i < m2_num; i++) {
				node_t *node = ll_get_nth_node(m_list, i + m_index + 1);
				miniblock_t *m = node->data;
				ll_add_nth_node(m2_list, i, m);
			}

			for (uint64_t i = 0; i < m_list->size; i++) {
				node_t *node = ll_get_nth_node(m_list, i);
				miniblock_t *m = node->data;
				free(m->rw_buffer);
			}

			ll_free(&m_list);
			node_t *rem_node = ll_remove_nth_node(arena->alloc_list, b_index);
			free(rem_node->data);
			free(rem_node);

			ll_add_nth_node(arena->alloc_list, b_index, b1);
			free(b1);
			ll_add_nth_node(arena->alloc_list, b_index + 1, b2);
			free(b2);
		}
	}
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	if (zone_is_free(arena, address, size)) {
		printf("Invalid address for read.\n");
		return;
	}

	// Determine the main block and check permissions
	block_t *block = block_containing_address(arena, address);
	list_t *m_list = block->miniblock_list;
	node_t *iterator = m_list->head;
	uint64_t m_index = index_miniblock(arena, address);
	for (uint64_t i = 0; i < m_list->size; i++) {
		miniblock_t *m = iterator->data;
		if (i >= m_index && !m->r) {
			printf("Invalid permissions for read.\n");
			return;
		}
		iterator = iterator->next;
	}

	uint64_t read_size = size,
			 b_start = block->start_address,
			 b_end = block->start_address + block->size,
			 steps = 0;

	if (b_end < address + read_size) {
		read_size = block->size;
		printf("Warning: size was bigger than the block size.");
		printf(" Reading %lu characters.\n", read_size);
	}

	// Printing the data from the address
	iterator = m_list->head;
	while (iterator && read_size) {
		miniblock_t *miniblock = iterator->data;
		char *buffer = miniblock->rw_buffer;
		if (!buffer)
			break;
		for (uint64_t i = 0; i < miniblock->size && read_size; i++) {
			if ((steps + b_start) >= address) {
				printf("%c", buffer[i]);
				read_size--;
			}
			steps++;
		}

		iterator = iterator->next;
	}

	if (!read_size)
		printf("\n");
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data)
{
	// Determine main block
	block_t *block = block_containing_address(arena, address);
	if (zone_is_free(arena, address, size) || !block) {
		printf("Invalid address for write.\n");
		return;
	}

	// Checking permissions
	list_t *m_list = block->miniblock_list;
	node_t *iterator = m_list->head;
	uint64_t m_index = index_miniblock(arena, address);
	for (uint64_t i = 0; i < m_list->size; i++) {
		miniblock_t *m = iterator->data;
		if (i >= m_index && !m->w) {
			printf("Invalid permissions for write.\n");
			return;
		}
		iterator = iterator->next;
	}

	uint64_t data_size = strlen((char *)data),
			 b_end = block->start_address + block->size;

	if (b_end < address + data_size) {
		data_size = b_end - address;
		printf("Warning: size was bigger than the block size. ");
		printf("Writing %lu characters.\n", data_size);
	}

	// Writing data at the address
	iterator = m_list->head;
	uint64_t count = data_size;
	while (iterator && count) {
		miniblock_t *miniblock = iterator->data;
		miniblock->rw_buffer = malloc(miniblock->size);
		DIE(!miniblock->rw_buffer, "write(): malloc() failed\n");

		uint64_t copy_size;
		if (miniblock->size <= count)
			copy_size = miniblock->size;
		else
			copy_size = count;

		memcpy(miniblock->rw_buffer, data + (data_size - count), copy_size);

		count -= copy_size;

		iterator = iterator->next;
	}
}

void pmap(const arena_t *arena)
{
	// Total memory
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);

	// Free memory
	list_t *b_list = arena->alloc_list;
	node_t *iterator = b_list->head;
	uint64_t size_occp_mem = 0;
	while (iterator) {
		block_t *block = iterator->data;
		size_occp_mem += block->size;
		iterator = iterator->next;
	}
	printf("Free memory: 0x%lX bytes\n", arena->arena_size - size_occp_mem);

	// Allocated blocks
	printf("Number of allocated blocks: %u\n", arena->alloc_list->size);

	// Allocated miniblocks
	iterator = b_list->head;
	uint64_t num_mb = 0;

	while (iterator) {
		block_t *block = iterator->data;
		list_t *m_list = block->miniblock_list;
		num_mb += m_list->size;
		iterator = iterator->next;
	}

	printf("Number of allocated miniblocks: %ld\n", num_mb);
	if (num_mb)
		printf("\n");

	// Block begin - end
	iterator = b_list->head;
	for (uint64_t i = 1; i <= b_list->size; i++) {
		block_t *block = iterator->data;
		printf("Block %ld begin\n", i);

		printf("Zone: 0x%lX - 0x%lX\n", block->start_address,
			   block->start_address + block->size);

		list_t *m_list = block->miniblock_list;
		node_t *m_iter = m_list->head;
		for (uint64_t j = 1; j <= m_list->size; ++j) {
			miniblock_t *miniblock = m_iter->data;
			printf("Miniblock %ld:", j);
			printf("\t\t0x%lX\t\t-\t\t0x%lX\t\t|", miniblock->start_address,
				   miniblock->start_address + miniblock->size);
			printf(" ");
			printf("%c", miniblock->r ? 'R' : '-');
			printf("%c", miniblock->w ? 'W' : '-');
			printf("%c\n", miniblock->x ? 'X' : '-');
			m_iter = m_iter->next;
		}

		iterator = iterator->next;

		printf("Block %ld end\n", i);
		if (i < b_list->size)
			printf("\n");
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	miniblock_t *miniblock = miniblock_at_address(arena, address);
	if (!miniblock) {
		printf("Invalid address for mprotect.\n");
		return;
	}

	uint64_t perm_value = 0;
	int8_t *perm = (int8_t *)strtok((char *)permission, " | ");
	uint8_t r = 0, w = 0, x = 0;
	while (perm) {
		if (strcmp((char *)perm, "PROT_READ") == 0) {
			perm_value += 4;
			r = 1;
		} else if (strcmp((char *)perm, "PROT_WRITE") == 0) {
			perm_value += 2;
			w = 1;
		} else if (strcmp((char *)perm, "PROT_EXEC") == 0) {
			perm_value += 1;
			x = 1;
		} else if (strcmp((char *)perm, "PROT_NONE") == 0) {
			perm_value = 0;
			break;
		}

		perm = (int8_t *)strtok(NULL, " | ");
	}

	miniblock->perm = perm_value;
	miniblock->r = r;
	miniblock->w = w;
	miniblock->x = x;
}
