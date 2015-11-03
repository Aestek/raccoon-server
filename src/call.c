#include <stdlib.h>
#include "call.h"
#include "lib/array_queue.h"

call* call_new(call_fn fn, void *arg, call *next)
{
	call *c = malloc(sizeof(call));

	c->fn = fn;
	c->arg = arg;
	c->next = next;
	c->err = 0;

	return c;
}

void call_exec(array_queue *q)
{
	call *c;
	int ret;
	for (;;) {
		if ((c = array_queue_pop(q)) == NULL)
			continue;

		ret = c->fn(q, c->err, c->arg, c->next);
		if (c->next != NULL && ret != 1) {
			if (ret < 0)
				c->next->err  = ret;

			array_queue_push(q, c->next);
		}

		free(c);
	}
}