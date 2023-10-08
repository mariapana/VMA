#pragma once

#include "ll.h"
#include "locators.h"

// VMA functions

/*
 * @brief -> allocates and initializes arena
 * @param -> size = size of arena a.k.a. total number of addresses
 *					(const uint64_t)
 * @return -> newly created arena stuct (arena_t *)
 */
arena_t *alloc_arena(const uint64_t size);

/*
 * @brief -> frees all resources of the arena (including blocks,
 *			 miniblocks and linked lists)
 * @param -> arena (arena_t *)
 * @return -> void
 */
void dealloc_arena(arena_t *arena);

/*
 * @brief -> creates and initializes a new miniblock struct
 * @param -> address = miniblock start adress (const uint64_t)
 * @param -> size = miniblock size (const uint64_t)
 * @return -> newly created miniblock
 */
miniblock_t *create_miniblock(const uint64_t address, const uint64_t size);

/*
 * @brief -> creates and initializes a new block struct
 * @param -> address = block start adress (const uint64_t)
 * @param -> size = block size (const uint64_t)
 * @return -> newly created block
 */
block_t *create_block(const uint64_t address, const uint64_t size);

/*
 * @brief -> implementation of alloc_block case of double adjacency
 * @param -> arena (arena_t *)
 * @param -> address = block start adress (uint64_t)
 * @param -> size = block size (uint64_t)
 * @param -> sh_start = index of the block sharing the new block's
 *						start address
 * @return -> void
 */
void double_adjacent(arena_t *arena, uint64_t address, uint64_t size,
					 uint64_t sh_start);

/*
 * @brief -> implementation of alloc_block case of left adjacency
 * @param -> arena (arena_t *)
 * @param -> address = block start adress (uint64_t)
 * @param -> size = block size (uint64_t)
 * @param -> sh_start = index of the block sharing the new block's
 *						start address
 * @return -> void
 */
void left_adjacent(arena_t *arena, uint64_t address, uint64_t size,
				   uint64_t sh_start);

/*
 * @brief -> implementation of alloc_block case of right adjacency
 * @param -> arena (arena_t *)
 * @param -> address = block start adress (uint64_t)
 * @param -> size = block size (uint64_t)
 * @param -> sh_end = index of the block sharing the new block's
 *					  end address
 * @return -> void
 */
void right_adjacent(arena_t *arena, uint64_t address, uint64_t size,
					uint64_t sh_end);

/*
 * @brief -> allocates a new block and adds to the arena list
 * @param -> arena (arena_t *)
 * @param -> address = block start adress (const uint64_t)
 * @param -> size = block size (const uint64_t)
 * @return -> void
 */
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);

/*
 * @brief -> frees block and its resources (miniblocks and miniblock list)
 * @param -> arena (arena_t *)
 * @param -> address = block start adress (const uint64_t)
 * @return -> void
 */
void free_block(arena_t *arena, const uint64_t address);

/*
 * @brief -> print data stored at given address in the arena
 * @param -> arena (arena_t *)
 * @param -> address = read start adress (uint64_t)
 * @param -> size = number of bytes to read (uint64_t)
 * @return -> void
 */
void read(arena_t *arena, uint64_t address, uint64_t size);

/*
 * @brief -> write data at given address in the arena
 * @param -> arena (arena_t *)
 * @param -> address = write start adress (const uint64_t)
 * @param -> size = number of bytes to write (const uint64_t)
 * @return -> void
 */
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data);

/*
 * @brief -> pmap function implementation
 * @param -> arena (arena_t *)
 * @return -> void
 */
void pmap(const arena_t *arena);

/*
 * @brief -> change permissions to the ones given
 * @param -> arena (arena_t *)
 * @param -> address = miniblock start adress (uint64_t)
 * @param -> pemission (int8_t *)
 * @return -> void
 */
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);
