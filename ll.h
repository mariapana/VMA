#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Macros
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

// Structs
typedef struct node_t node_t;
struct node_t {
	void *data;
	node_t *prev;
	node_t *next;
};

typedef struct {
	node_t *head;
	unsigned int data_size;
	unsigned int size;
} list_t;

// Linked list functions

/*
 * @brief -> creates a new doubly linked list
 * @param -> data_size = size of data to be stored in the list (unsigned int)
 * @return -> newly created doubly linked list (list_t *)
 */
list_t *ll_create(unsigned int data_size);

/*
 * @brief -> creates a new node
 * @param -> data_size = size of data to be stored in the node (unsigned int)
 * @param -> new_data = info to be stored in the node (const void *)
 * @return -> newly created node (node_t *)
 */
node_t *node_create(unsigned int data_size, const void *data);

/*
 * @brief -> finds the nth node in a given list
 * @param -> list = the list where we search for the node (list_t *)
 * @param -> n = the index of the desired node
 * @return -> the node at the given index
 */
node_t *ll_get_nth_node(list_t *list, unsigned int n);

/*
 * @brief -> adds a new node at index n in a given list
 * @param -> list = the list where the node is added (list_t *)
 * @param -> n = the index (unsigned int)
 * @param -> new_data = info to be stored in the node (const void *)
 * @return -> the node at the given index
 */
void ll_add_nth_node(list_t *list, unsigned int n, const void *data);

/*
 * @brief -> removes the nth node of a given list
 * @param -> list = the list from which we delete nodes (list_t *)
 * @param -> n = the index of the node to be removed (unsigned int)
 * @return -> the now removed node
 */
node_t *ll_remove_nth_node(list_t *list, unsigned int n);

/*
 * @brief -> frees all allocated resources of the list
 * @param -> list = the list from which we delete nodes (list_t **)
 * @return -> void
 */
void ll_free(list_t **list);
