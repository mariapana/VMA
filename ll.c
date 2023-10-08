#include "ll.h"

list_t *ll_create(unsigned int data_size)
{
	list_t *list = malloc(sizeof(list_t));
	DIE(!list, "ll_create(): malloc() failed\n");
	list->data_size = data_size;
	list->head = NULL;
	list->size = 0;
	return list;
}

node_t *node_create(unsigned int data_size, const void *data)
{
	node_t *new_node = malloc(sizeof(node_t));
	DIE(!new_node, "node_create(): malloc() failed\n");

	new_node->next = NULL;
	new_node->prev = NULL;

	new_node->data = malloc(data_size);
	DIE(!new_node, "malloc() failed\n");
	memcpy(new_node->data, data, data_size);

	return new_node;
}

node_t *ll_get_nth_node(list_t *list, unsigned int n)
{
	DIE(!list, "List needs to be created first\n");

	DIE(list->size == 0, "List should contain at least one node\n");

	if (n >= list->size)
		n = n % list->size;

	node_t *node = list->head;

	if (!n)
		return node;

	for (unsigned int i = 0; i < n; i++)
		node = node->next;

	return node;
}

void ll_add_nth_node(list_t *list, unsigned int n, const void *data)
{
	DIE(!list, "List needs to be created first\n");

	if (n >= list->size)
		n = list->size;

	node_t *new_node = node_create(list->data_size, data);

	if (list->size == 0) {
		list->head = new_node;
		new_node->next = NULL;
		new_node->prev = list->head;
	} else {
		if (n == 0) {
			new_node->next = list->head;
			new_node->prev = NULL;
			list->head = new_node;
		} else {
			node_t *prev_node = ll_get_nth_node(list, n - 1);
			new_node->prev = prev_node;
			new_node->next = prev_node->next;
			prev_node->next = new_node;
			if (n != list->size)
				new_node->next->prev = new_node;
		}
	}

	list->size++;
}

node_t *ll_remove_nth_node(list_t *list, unsigned int n)
{
	DIE(!list, "List needs to be created first\n");
	DIE(!list->size, "List should contain at least one node\n");

	if (n >= list->size - 1)
		n = list->size - 1;

	node_t *rem_node = ll_get_nth_node(list, n);
	node_t *right = rem_node->next;
	node_t *left = rem_node->prev;

	if (!list->size) {
		list->head = NULL;
		list->size--;
		return rem_node;
	}

	if (!n) {
		list->head = right;
		list->size--;
		return rem_node;
	}

	if (left)
		left->next = right;
	if (right)
		right->prev = left;

	list->size--;
	return rem_node;
}

void ll_free(list_t **list)
{
	DIE(!(*list), "List needs to be created first\n");

	while ((*list)->size) {
		node_t *node = ll_remove_nth_node(*list, 0);
		free(node->data);
		free(node);
	}

	free(*list);
	list = NULL;
}
