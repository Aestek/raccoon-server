#include <stdlib.h>
#include <stdio.h>
#include "array_queue.h"

void array_queue_init(array_queue *q, int size)
{
	q->elements = malloc(size * sizeof(void*));
	q->head = 0;
	q->tail = -1;
	q->size = size;
	q->length = 0;
}

array_queue *array_queue_new(int size)
{
	array_queue *q = malloc(sizeof(array_queue));
	array_queue_init(q, size);
	return q;
}

int array_queue_size(array_queue *q)
{
	return q->length;
}

void array_queue_push(array_queue *q, void *el)
{
	int pos = (q->tail + 1) % q->size;
	q->elements[pos] = el;
	q->tail = pos;
	q->length++;
}

void *array_queue_pop(array_queue *q)
{
	if (q->length == 0)
		return NULL;

	void *el = q->elements[q->head];
	q->head = (q->head + 1) % q->size;
	q->length--;
	return el;
}