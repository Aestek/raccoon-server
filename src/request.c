#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "http_request.h"

void http_request_parse_uri(request_uri *uri, char path[])
{
	int path_end_index = REQUEST_PATH_MAX_LENGTH;
	char *question_mark = strchr(path, '?');

	if (question_mark != NULL)
		path_end_index = (int)(question_mark - path);

	strncpy(uri->path, path, path_end_index);

	if (question_mark != NULL)
		strncpy(uri->query_string, &path[path_end_index + 1], QUERY_STRING_MAX_LENGTH);
	else
		uri->query_string[0] = '\0';
}


void http_request_parse_inner(http_request *req, char payload[], int payload_size)
{
	int space_pos = 0;
	int start_index = 0;
	char path[REQUEST_PATH_MAX_LENGTH];

	bzero(&(req->method), sizeof(req->method));
	bzero(&(path), sizeof(path));
	bzero(&(req->http_version_str), sizeof(req->http_version_str));

	/** todo refac **/
	for (int i = 0; i < payload_size; i++) {
		if (payload[i] == '\n' || payload[i] == '\r') {
			break;
		} else if (payload[i] == ' ') {
			space_pos++;
			start_index = i + 1;
		} else {
			switch (space_pos) {
				case 0:
					req->method[i] = payload[i];

					if (i > 3)
						perror("Method too long");
					break;

				case 1:
					path[i - start_index] = payload[i];
					break;

				case 2:
					req->http_version_str[i - start_index] = payload[i];
					break;
			}
		}
	}

	req->uri = malloc(sizeof(request_uri));
	http_request_parse_uri(req->uri, path);
}

http_request *http_request_parse(int client_sockfd)
{
	const int input_buffer_size = 2052;
	char input_buffer[input_buffer_size];
	int input_payload_size;

	input_payload_size = read(client_sockfd, input_buffer, input_buffer_size);
	http_request *req = malloc(sizeof(http_request));
	req->client_sockfd = client_sockfd;
	http_request_parse_inner(req, input_buffer, input_payload_size);

	return req;
}