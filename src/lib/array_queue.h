#pragma once

typedef struct {
	int head;
	int tail;
	int size;
	void **elements;
	int length;
} array_queue;

void array_queue_init(array_queue *q, int size);
array_queue *array_queue_new(int size);
int array_queue_size(array_queue *q);
void array_queue_push(array_queue *q, void *el);
void *array_queue_pop(array_queue *q);

