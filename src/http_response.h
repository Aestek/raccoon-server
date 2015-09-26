#pragma once

#include "lib/linked_list.h"

typedef void (*response_body_writer)(int, void*);

typedef struct
{
	char http_version_str[9];
	int status_code;
	list headers;
	void *body_data;
	response_body_writer response_body_writer;
} http_response;

http_response* http_response_new();