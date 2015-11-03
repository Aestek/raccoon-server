#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "response.h"
#include "call.h"
#include "lib/array_queue.h"

http_response* http_response_new()
{
	http_response *res = malloc(sizeof(http_response));
	res->body_data = NULL;
	res->output_buffer_written = 0;
	list_new(&res->headers, &free);
	return res;
}

void http_response_destroy(http_response *res)
{
	free(res->body_data);
	list_destroy(&res->headers);
	free(res);
}

int http_response_write(array_queue *equeue, int err, void *arg, call *next)
{
	http_response *res = (http_response*)arg;
	next->arg = res;

	if (res->output_buffer_written == 0) {
		sprintf(res->output_buffer, "%s %d Raccoon\r\n", res->http_version_str, res->status_code);

		list_iterator *it = list_get_iterator(&res->headers);
		http_header *header;
		while ((header = list_iterate(it)) != NULL) {
			strcat(res->output_buffer, header->name);
			strcat(res->output_buffer, ":");
			strcat(res->output_buffer, header->value);
			strcat(res->output_buffer, "\r\n");
		}

		strcat(res->output_buffer, "\r\n");
	}

	int output_buffer_len = strlen(res->output_buffer);

	int witten = write(res->client_sockfd, res->output_buffer, output_buffer_len);


	if (witten > 0)
		res->output_buffer_written += witten;

	if ((witten == -1 && errno == EAGAIN) || (witten > 0 && res->output_buffer_written < output_buffer_len)) {
		array_queue_push(equeue, call_new(&http_response_write, res, next));
		return 1;
	} else if (witten == -1) {
		return -2;
	}

	if (res->body_data != NULL && &res->response_body_writer != NULL)
		res->response_body_writer(res->client_sockfd, res->body_data);

	return 0;
}