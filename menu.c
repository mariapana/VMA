#include "vma.h"
#include "menu.h"

void write_menu(arena_t *arena)
{
	uint64_t address;
	scanf("%ld", &address);

	uint64_t size;
	scanf("%ld", &size);

	uint8_t leading_space;
	scanf("%c", &leading_space);

	int8_t *data = malloc(size + 1);
	DIE(!data, "write_menu(): malloc() failed\n");

	char ch;
	for (uint64_t i = 0; i < size; i++) {
		scanf("%c", &ch);
		data[i] = ch;
	}

	data[size] = '\0';

	write(arena, address, size, data);

	free(data);
}

void mprotect_menu(arena_t *arena)
{
	uint64_t address;
	scanf("%ld", &address);
	uint8_t leading_space;
	scanf("%c", &leading_space);
	int8_t *permission = malloc(L_MAX);
	DIE(!permission, "menu(): malloc() failed\n");
	fgets((char *)permission, L_MAX, stdin);
	if (permission[strlen((char *)permission) - 1] == '\n')
		permission[strlen((char *)permission) - 1] = '\0';
	mprotect(arena, address, permission);
	free(permission);
}

void invalid_menu(void)
{
	printf("Invalid command. Please try again.\n");

	char *rest_of_cmd = malloc(L_MAX);
	DIE(!rest_of_cmd, "invalid_menu(): malloc() failed\n");
	fgets(rest_of_cmd, L_MAX, stdin);

	for (size_t i = 0; i < strlen(rest_of_cmd); i++)
		if (rest_of_cmd[i] == '\n')
			rest_of_cmd[i] = '\0';

	char *p = strtok(rest_of_cmd, " ");
	while (p) {
		printf("Invalid command. Please try again.\n");
		p = strtok(NULL, " ");
	}

	free(rest_of_cmd);
}

int menu(void)
{
	arena_t *arena = NULL;

	char *cmd = malloc(CMD_MAX);
	DIE(!cmd, "menu(): malloc() failed\n");

	while (scanf("%s", cmd)) {
		if (strcmp(cmd, "ALLOC_ARENA") == 0) {
			uint64_t size;
			scanf("%ld", &size);
			arena = alloc_arena(size);
		} else if (strcmp(cmd, "DEALLOC_ARENA") == 0) {
			dealloc_arena(arena);
			free(cmd);
			return 0;
		} else if (strcmp(cmd, "ALLOC_BLOCK") == 0) {
			uint64_t address;
			scanf("%ld", &address);
			uint64_t size;
			scanf("%ld", &size);
			alloc_block(arena, address, size);
		} else if (strcmp(cmd, "FREE_BLOCK") == 0) {
			uint64_t address;
			scanf("%ld", &address);
			free_block(arena, address);
		} else if (strcmp(cmd, "READ") == 0) {
			uint64_t address;
			scanf("%ld", &address);
			uint64_t size;
			scanf("%ld", &size);
			read(arena, address, size);
		} else if (strcmp(cmd, "WRITE") == 0) {
			write_menu(arena);
		} else if (strcmp(cmd, "PMAP") == 0) {
			pmap(arena);
		} else if (strcmp(cmd, "MPROTECT") == 0) {
			mprotect_menu(arena);
		} else {
			invalid_menu();
		}
	}

	return 0;
}
