#pragma once

#include "lib/linked_list.h"
#include "lib/array_queue.h"
#include "conf.h"
#include "header.h"
#include "request.h"
#include "call.h"

typedef void (*response_body_writer)(int, void*);

typedef struct
{
	char http_version_str[HTTP_VERSION_STR_MAX_LENGTH];
	int status_code;
	list headers;
	void *body_data;
	response_body_writer response_body_writer;
	char output_buffer[40000];
	int output_buffer_written;
	int client_sockfd;
} http_response;

http_response* http_response_new();
void http_response_destroy(http_response *res);

int http_response_write(array_queue *equeue, int err, void *arg, call *next);