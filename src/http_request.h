#pragma once

typedef struct
{
	char method[5];
	char path[2048];
	char http_version_str[9];
	int client_sockfd;
} http_request;
