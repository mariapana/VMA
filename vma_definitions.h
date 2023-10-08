#pragma once
#include <inttypes.h>
#include <stddef.h>

#include "ll.h"

// Macros
#define CMD_MAX 14
#define L_MAX 200

// Structs
typedef struct {
	uint64_t start_address;
	size_t size;
	list_t *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm, r, w, x;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
} arena_t;
