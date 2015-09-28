#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "request.h"

http_request* http_request_new()
{
	return malloc(sizeof(http_request));
}

void http_request_parse_uri(http_request *req, char path[])
{
	int path_end_index = REQUEST_PATH_MAX_LENGTH;
	char *question_mark = strchr(path, '?');

	if (question_mark != NULL)
		path_end_index = (int)(question_mark - path);

	strncpy(req->path, path, path_end_index);

	if (question_mark != NULL)
		strncpy(req->query_string, &path[path_end_index + 1], QUERY_STRING_MAX_LENGTH);
	else
		req->query_string[0] = '\0';
}

int http_request_parse(http_request *req, int client_sockfd)
{
	const int input_buffer_size =
		HTTP_METHOD_MAX_LENGTH +
		REQUEST_PATH_MAX_LENGTH +
		QUERY_STRING_MAX_LENGTH +
		HTTP_VERSION_STR_MAX_LENGTH;

	char input_buffer[input_buffer_size];

	read(client_sockfd, input_buffer, input_buffer_size);
	req->client_sockfd = client_sockfd;

	char payload_delim[] = " ";
	char *playload_parts;

	char *method = strtok_r(input_buffer, payload_delim, &playload_parts);

	if (method == NULL)
		return -1;

	strncpy(req->method, method, HTTP_METHOD_MAX_LENGTH);

	char *path = strtok_r(NULL, payload_delim, &playload_parts);

	if (path == NULL)
		return -1;

	char *http_ver =  strtok_r(NULL, payload_delim, &playload_parts);

	if (http_ver != NULL)
		strncpy(req->http_version_str, http_ver, HTTP_VERSION_STR_MAX_LENGTH);
	else
		strcpy(req->http_version_str, DEFAULT_HTTP_VERSION_STR);

	http_request_parse_uri(req, path);

	return 0;
}