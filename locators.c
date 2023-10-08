#include "locators.h"

uint8_t zone_is_free(arena_t *arena, const uint64_t address,
					 const uint64_t size)
{
	list_t *b_list = arena->alloc_list;

	if (!b_list->head)
		return 1;

	node_t *iterator = b_list->head;
	while (iterator) {
		block_t *block = iterator->data;
		uint64_t end = address + size,
				 b_end = block->start_address + block->size;

		if (b_list->head == iterator && end <= block->start_address)
			return 1;

		if (iterator->next) {
			block_t *next_b = iterator->next->data;
			if (address >= b_end && end <= next_b->start_address)
				return 1;
		}

		if (address == block->start_address && end == b_end)
			return 0;

		if (block->start_address < address && end <= b_end)
			return 0;

		if (block->start_address < address && address < b_end)
			return 0;

		if (block->start_address <= end && end <= b_end)
			return 0;

		if (address > block->start_address && !iterator->next)
			return 1;

		iterator = iterator->next;
	}

	return 0;
}

int64_t share_start(list_t *b_list, const uint64_t start_address)
{
	node_t *iterator = b_list->head;
	int64_t index = 0;

	while (iterator) {
		block_t *block = iterator->data;
		if (start_address == block->start_address + block->size)
			return index;
		iterator = iterator->next;
		index++;
	}

	return -1;
}

int64_t share_end(list_t *b_list, const uint64_t end_address)
{
	node_t *iterator = b_list->head;
	int64_t index = 0;

	while (iterator) {
		block_t *block = iterator->data;
		if (end_address == block->start_address)
			return index;
		iterator = iterator->next;
		index++;
	}

	return -1;
}

// Index in b_list nearest to the new block address (not allocated yet)
uint64_t index_last_block_from_address(arena_t *arena, const uint64_t address)
{
	list_t *b_list = arena->alloc_list;
	node_t *iterator = b_list->head;

	uint64_t i;
	for (i = 0; i < b_list->size; i++) {
		block_t *block = iterator->data;

		if (address <= block->start_address)
			return i - 1;

		iterator = iterator->next;
	}

	return i;
}

block_t *block_containing_address(arena_t *arena, const uint64_t address)
{
	list_t *b_list = arena->alloc_list;
	node_t *b_iter = b_list->head;

	while (b_iter) {
		block_t *block = b_iter->data;
		uint64_t b_end_address = block->start_address + block->size;

		if (block->start_address <= address && address <= b_end_address)
			return block;

		b_iter = b_iter->next;
	}

	return NULL;
}

// Index in b_list for allocated blocks
int64_t index_block(arena_t *arena, const uint64_t address)
{
	list_t *b_list = arena->alloc_list;
	node_t *b_iter = b_list->head;
	uint64_t b_index = -1;

	while (b_iter) {
		block_t *block = b_iter->data;
		uint64_t b_end_address = block->start_address + block->size - 1;
		b_index++;

		if (block->start_address <= address && address <= b_end_address)
			return b_index;

		b_iter = b_iter->next;
	}

	return b_index;
}

miniblock_t *miniblock_at_address(arena_t *arena, const uint64_t address)
{
	list_t *b_list = arena->alloc_list;
	node_t *b_iter = b_list->head;

	while (b_iter) {
		block_t *block = b_iter->data;
		list_t *m_list = block->miniblock_list;
		node_t *m_iter = m_list->head;

		while (m_iter) {
			miniblock_t *m = m_iter->data;

			if (m->start_address == address)
				return m;

			m_iter = m_iter->next;
		}

		b_iter = b_iter->next;
	}

	return NULL;
}

int64_t index_miniblock(arena_t *arena, const uint64_t address)
{
	block_t *block = block_containing_address(arena, address);
	uint64_t m_index = -1;

	if (block) {
		list_t *m_list = block->miniblock_list;
		node_t *m_iter = m_list->head;

		while (m_iter) {
			miniblock_t *miniblock = m_iter->data;
			m_index++;

			if (address == miniblock->start_address)
				return m_index;

			m_iter = m_iter->next;
		}
	}

	return m_index;
}

// The sum of miniblock sizes within a given interval
uint64_t miniblock_size_sum(list_t *m_list, uint64_t start_index,
							const uint64_t end_index)
{
	node_t *iterator = ll_get_nth_node(m_list, start_index);
	miniblock_t *miniblock = iterator->data;

	if (start_index == end_index)
		return miniblock->size;

	uint64_t sum = 0;

	while (iterator && start_index <= end_index) {
		miniblock = iterator->data;
		sum += miniblock->size;
		start_index++;
		iterator = iterator->next;
	}

	return sum;
}

// The miniblock count in a given interval
uint64_t miniblock_num(list_t *m_list, uint64_t start_index,
					   const uint64_t end_index)
{
	node_t *iterator = ll_get_nth_node(m_list, start_index);

	if (start_index == end_index)
		return 1;

	uint64_t num = 0;

	while (iterator && start_index <= end_index) {
		num++;
		start_index++;
		iterator = iterator->next;
	}

	return num;
}
