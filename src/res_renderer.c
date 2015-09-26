#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http_response.h"
#include "http_request.h"
#include "http_common.h"
#include "lib/linked_list.h"

void write_crlf(http_request *req)
{
	write(req->client_sockfd, "\r\n", 2);
}

void write_header(http_request *req, http_header *header)
{
	char field[1284];
	strcpy(field, header->name);
	strcat(field, ": ");
	strcat(field, header->value);
	strcat(field, "\r\n");

	write(req->client_sockfd, field, strlen(field));
}

void write_response(http_request *req, http_response *res)
{
	char status_line[256];
	sprintf(status_line, "%s %d\r\n", res->http_version_str, res->status_code);
	write(req->client_sockfd, status_line, strlen(status_line));

	while (list_size(&res->headers)) {
		http_header *header = (http_header*)list_head(&res->headers, TRUE);
		write_header(req, header);
	}

	write_crlf(req);

	if (res->body_data != NULL)
		res->response_body_writer(req->client_sockfd, res->body_data);
}