#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include "header.h"
#include "request.h"
#include "lib/linked_list.h"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void run_server(int portno, http_handler handler, logger logger)
{
	const int backlog_size = TCP_BACKLOG_SIZE;
	int server_sockfd, client_sockfd;
	socklen_t client_addr_length;
	struct sockaddr_in serv_addr, cli_addr;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(
		server_sockfd,
		(struct sockaddr *) &serv_addr,
		sizeof(serv_addr)
	) < 0)
		error("ERROR on binding");

	listen(server_sockfd, backlog_size);

	client_addr_length = sizeof(cli_addr);

	for (;;) {
		client_sockfd = accept(
			server_sockfd,
			(struct sockaddr *) &cli_addr,
			&client_addr_length
		);

		if (client_sockfd < 0)
			error("ERROR on accept");

		http_request *req = http_request_parse(client_sockfd);
		http_response *res = http_response_new();
		strcpy(res->http_version_str, "HTTP/1.0");

		handler(req, res);
		http_response_write(req, res);
		logger(req, res);

		close(client_sockfd);
	}

	close(server_sockfd);
}

