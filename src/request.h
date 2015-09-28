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

http_request* http_request_new();

void http_request_parse_uri(http_request *req, char path[]);

int http_request_parse(http_request *req, int client_sockfd);