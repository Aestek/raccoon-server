#include <stdio.h>
#include <stdlib.h>
#include "http_response.h"

http_response* http_response_new()
{
	http_response *res = malloc(sizeof(http_response));
	list_new(&res->headers, NULL);
	return res;
}

void http_response_write_crlf(http_request *req)
{
	write(req->client_sockfd, "\r\n", 2);
}

void http_response_write_header(http_request *req, http_header *header)
{
	int field_len = HEADER_NAME_LEN + HEADER_VALUE_LEN + 2;
	char *field = malloc(field_len * sizeof(char));
	strcpy(field, header->name);
	strcat(field, ": ");
	strcat(field, header->value);
	strcat(field, "\r\n");

	write(req->client_sockfd, field, field_len);
	free(field);
}

void http_response_write(http_request *req, http_response *res)
{
	char status_line[256];
	sprintf(status_line, "%s %d\r\n", res->http_version_str, res->status_code);
	write(req->client_sockfd, status_line, strlen(status_line));

	while (list_size(&res->headers)) {
		http_header *header = (http_header*)list_head(&res->headers, TRUE);
		http_response_write_header(req, header);
	}

	http_response_write_crlf(req);

	if (res->body_data != NULL)
		res->response_body_writer(req->client_sockfd, res->body_data);
}