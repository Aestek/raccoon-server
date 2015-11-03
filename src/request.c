#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "request.h"
#include "lib/array_queue.h"
#include "call.h"

http_request* http_request_new()
{
	http_request *req = malloc(sizeof(http_request));
	req->input_buffer_read = 0;
	return req;
}

void http_request_parse_uri(http_request *req, char path[])
{
	int path_end_index = REQUEST_PATH_MAX_LENGTH;
	char *question_mark = strchr(path, '?');

	if (question_mark != NULL)
		path_end_index = (int)(question_mark - path);

	strncpy(req->path, path, path_end_index);
	req->path[path_end_index] = '\0';

	if (question_mark != NULL)
		strncpy(req->query_string, &path[path_end_index + 1], QUERY_STRING_MAX_LENGTH);
	else
		req->query_string[0] = '\0';
}

int http_request_parse(array_queue *equeue, int err, void *arg, call *next)
{
	http_request *req = (http_request*)arg;
	next->arg = req;

	if (req->input_buffer_read) {
		memset(&req->input_buffer, 0, 40000);
	}

	int last_read = read(req->client_sockfd, &req->input_buffer[req->input_buffer_read], 40000);

	if (last_read > 0)
		req->input_buffer_read += last_read;

	if ((last_read == -1 && errno == EAGAIN) || (last_read > 0 && strchr(req->input_buffer, '\r') == NULL)) {
		array_queue_push(equeue, call_new(&http_request_parse, req, next));
		return 1;
	} else if (last_read == -1) {
		perror("Parse");
		if (errno == EBADF)
			return -3;
		return -2;
	}

	char payload_delim[] = " ";
	char *playload_parts;

	char *method = strtok_r(req->input_buffer, payload_delim, &playload_parts);

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