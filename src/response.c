#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "response.h"

http_response* http_response_new()
{
	http_response *res = malloc(sizeof(http_response));
	res->body_data = NULL;
	list_new(&res->headers, &free);
	return res;
}

void http_response_destroy(http_response *res)
{
	free(res->body_data);
	list_destroy(&res->headers);
	free(res);
}

void http_response_write(http_request *req, http_response *res)
{
	int response_b_len = 1024;
	char response[response_b_len];
	sprintf(response, "%s %d Raccoon\r\n", res->http_version_str, res->status_code);

	list_iterator *it = list_get_iterator(&res->headers);
	http_header *header;
	while ((header = list_iterate(it)) != NULL) {
		strcat(response, header->name);
		strcat(response, ":");
		strcat(response, header->value);
		strcat(response, "\r\n");
	}

	strcat(response, "\r\n");

	write(req->client_sockfd, response, strlen(response));

	if (res->body_data != NULL && &res->response_body_writer != NULL)
		res->response_body_writer(req->client_sockfd, res->body_data);
}