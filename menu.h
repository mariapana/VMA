#pragma once

#include "vma.h"
#include "ctype.h"

void write_menu(arena_t *arena);

void mprotect_menu(arena_t *arena);

void invalid_menu(void);

/*
 * @brief -> command interface
 * @return -> the value that the main function will return (0)
 */
int menu(void);
