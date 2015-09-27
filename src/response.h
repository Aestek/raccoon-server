#pragma once

#include "lib/linked_list.h"
#include "conf.h"
#include "header.h"
#include "request.h"

typedef void (*response_body_writer)(int, void*);

typedef struct
{
	char http_version_str[HTTP_VERSION_STR_MAX_LENGTH];
	int status_code;
	list headers;
	void *body_data;
	response_body_writer response_body_writer;
} http_response;

http_response* http_response_new();

void http_response_write_crlf(http_request *req);

void http_response_write_header(http_request *req, http_header *header);

void http_response_write(http_request *req, http_response *res);