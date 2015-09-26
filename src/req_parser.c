#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <strings.h>
#include "http_request.h"

void parse_payload(http_request *req, char payload[], int payload_size)
{
	int space_pos = 0;
	int start_index = 0;

	bzero(&(req->method), sizeof(req->method));
	bzero(&(req->path), sizeof(req->path));
	bzero(&(req->http_version_str), sizeof(req->http_version_str));

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
					req->path[i - start_index] = payload[i];
					break;

				case 2:
					req->http_version_str[i - start_index] = payload[i];
					break;
			}
		}
	}
}

http_request *parse_request(int client_sockfd)
{
	const int input_buffer_size = 2052;
	char input_buffer[input_buffer_size];
	int input_payload_size;

	input_payload_size = read(client_sockfd, input_buffer, input_buffer_size);
	http_request *req = malloc(sizeof(http_request));
	req->client_sockfd = client_sockfd;
	parse_payload(req, input_buffer, input_payload_size);

	return req;
}