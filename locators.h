#pragma once

#include "vma_definitions.h"

/*
 * @brief -> checks if the given interval is already allocated
 * @param -> arena (arena_t *)
 * @param -> address = miniblock start adress (const uint64_t)
 * @param -> size = miniblock size (const uint64_t)
 * @return -> 1 if true, else 0
 */
uint8_t zone_is_free(arena_t *arena, const uint64_t address,
					 const uint64_t size);

/*
 * @brief -> checks if the new block will share the start address
 *			 with another block and returns the index
 * @param -> b_list = block list a.k.a. alloc_list (list_t *)
 * @param -> start_address (const uint64_t)
 * @return -> the neighbour block's index
 */
int64_t share_start(list_t *b_list, const uint64_t start_address);

/*
 * @brief -> checks if the new block will share the end address
 *			 with another block and returns the index
 * @param -> b_list = block list a.k.a. alloc_list (list_t *)
 * @param -> end_address (const uint64_t)
 * @return -> the neighbour block's index
 */
int64_t share_end(list_t *b_list, const uint64_t end_address);

/*
 * @brief -> determine the index of the last block before
 *			 the given address
 * @param -> arena (arena_t *)
 * @param -> address (const uint64_t)
 * @return -> the block's index
 */
uint64_t index_last_block_from_address(arena_t *arena, const uint64_t address);

/*
 * @brief -> determine the block containing the given address
 * @param -> arena (arena_t *)
 * @param -> address (const uint64_t)
 * @return -> the block
 */
block_t *block_containing_address(arena_t *arena, const uint64_t address);

/*
 * @brief -> determine the index of the block in alloc_list
 * @param -> arena (arena_t *)
 * @param -> address (const uint64_t)
 * @return -> the block's index
 */
int64_t index_block(arena_t *arena, const uint64_t address);

/*
 * @brief -> determine the miniblock at the given address
 * @param -> arena (arena_t *)
 * @param -> address (const uint64_t)
 * @return -> the miniblock
 */
miniblock_t *miniblock_at_address(arena_t *arena, const uint64_t address);

/*
 * @brief -> determine the index of the miniblock in miniblock list
 * @param -> arena (arena_t *)
 * @param -> address (const uint64_t)
 * @return -> the miniblock's index
 */
int64_t index_miniblock(arena_t *arena, const uint64_t address);

/*
 * @brief -> sum of miniblock sizes within a given interval in m_list
 * @param -> m_list = miniblock list (list_t *)
 * @param -> start_index (uint64_t)
 * @param -> end_index (const uint64_t)
 * @return -> value of sum
 */
uint64_t miniblock_size_sum(list_t *m_list, uint64_t start_index,
							const uint64_t end_index);

/*
 * @brief -> counts miniblocks within a given interval in m_list
 * @param -> m_list = miniblock list (list_t *)
 * @param -> start_index (uint64_t)
 * @param -> end_index (const uint64_t)
 * @return -> number of miniblocks
 */
uint64_t miniblock_num(list_t *m_list, uint64_t start_index,
					   const uint64_t end_index);
