#pragma once

#include "conf.h"

typedef struct {
	char path[REQUEST_PATH_MAX_LENGTH];
	char query_string[QUERY_STRING_MAX_LENGTH];
} request_uri;

typedef struct
{
	char method[HTTP_METHOD_MAX_LENGTH];
	char path[REQUEST_PATH_MAX_LENGTH];
	char query_string[QUERY_STRING_MAX_LENGTH];
	char http_version_str[HTTP_VERSION_STR_MAX_LENGTH];
	int client_sockfd;
} http_request;

void http_request_parse_uri(request_uri *uri, char path[]);

http_request *http_request_parse(int client_sockfd);