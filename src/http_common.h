#pragma once

#include "http_response.h"
#include "http_request.h"

typedef struct {
	char name[256];
	char value[1024];
} http_header;

typedef void (*http_handler)(http_request*, http_response*);

void add_http_header(http_response *response, http_header *header);

http_header* header_new(char name[], char value[]);
