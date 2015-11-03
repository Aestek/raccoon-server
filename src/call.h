#pragma once

#include "lib/array_queue.h"

typedef struct call {
	int (*fn)(array_queue*, int, void*, struct call*);
	int err;
	void *arg;
	struct call *next;
} call;

typedef int (*call_fn)(array_queue*, int, void*, call*);

call* call_new(call_fn fn, void *arg, call *next);

void call_exec(array_queue *q);