#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "linked_list.h"

void list_new(list *list, freeFunction freeFn)
{
	list->logicalLength = 0;
	list->head = list->tail = NULL;
	list->freeFn = freeFn;
}

void list_destroy(list *list)
{
	listNode *current;
	while(list->head != NULL) {
		current = list->head;
		list->head = current->next;

		if(list->freeFn) {
			list->freeFn(current->data);
		}
		free(current);
	}
}

void list_prepend(list *list, void *element)
{
	listNode *node = malloc(sizeof(listNode));
	node->data = element;

	node->next = list->head;
	list->head = node;

	// first node?
	if (!list->tail) {
		list->tail = list->head;
	}

	list->logicalLength++;
}

void list_append(list *list, void *element)
{
	listNode *node = malloc(sizeof(listNode));
	node->data = element;
	node->next = NULL;

	if(list->logicalLength == 0) {
		list->head = list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}

	list->logicalLength++;
}

void *list_head(list *list, bool removeFromList)
{
	assert(list->head != NULL);

	listNode *node = list->head;
	void *data = node->data;

	if(removeFromList) {
		list->head = node->next;
		list->logicalLength--;
		free(node);
	}

	return data;
}

void *list_tail(list *list)
{
	assert(list->tail != NULL);
	listNode *node = list->tail;
	return node->data;
}

int list_size(list *list)
{
	return list->logicalLength;
}

list_iterator *list_get_iterator(list *list)
{
	list_iterator *it = malloc(sizeof(list_iterator));
	*it = list->head;
	return it;
}

void *list_iterate(list_iterator *it)
{
	listNode *node = *it;

	if (node == NULL)
		return NULL;

	*it = node->next;
	return node->data;
}