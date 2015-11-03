#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include "server.h"
#include "header.h"
#include "request.h"
#include "lib/linked_list.h"
#include "lib/array_queue.h"
#include "call.h"

http_handler handler;
logger log_handler;
int server_sockfd;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void signal_callback_handler(int signum)
{
	printf("Caught signal SIGPIPE %d\n",signum);
}

static int clean_up(array_queue *equeue, int err, void *arg, call *next)
{
	http_response *res = (http_response*)arg;
	close(res->client_sockfd);
	http_response_destroy((http_response*)arg);
	return 0;
}

static int process_connection(array_queue *equeue, int err, void *arg, call *next)
{
	http_response *res = http_response_new();
	http_request *req = (http_request*)arg;
	strcpy(res->http_version_str, "HTTP/1.1");
	res->client_sockfd = req->client_sockfd;

	if (err == 0) {
		handler(req, res);
	} else if (err == -3) {
		array_queue_push(equeue, call_new(&clean_up, res, NULL));
		printf("Bad req\n");
		return 1;
	} else {
		res->status_code = 400;
	}

	if (res->status_code != 200)
		log_handler(req, res);
	free(req);

	next->arg = res;

	return 0;
}

static int handle_connection(array_queue *equeue, int err, void *arg, call *next)
{
	struct sockaddr_in cli_addr;
	socklen_t client_addr_length = sizeof(cli_addr);

	int sock_fd = accept(
		server_sockfd,
		(struct sockaddr *) &cli_addr,
		&client_addr_length
	);

	if (sock_fd >= 0) {
		int flags = fcntl(sock_fd, F_GETFL, 0);
		fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);

		http_request *req = http_request_new();
		req->client_sockfd = sock_fd;

		array_queue_push(equeue,
			call_new(&http_request_parse, req,
				call_new(&process_connection, NULL,
					call_new(&http_response_write, NULL,
						call_new(&clean_up, NULL, NULL)))));
	}

	array_queue_push(equeue, call_new(&handle_connection, NULL, NULL));

	return 0;
}

static void tr_handler(void *ptr)
{
	array_queue *equeue = array_queue_new(1024);
	array_queue_push(equeue, call_new(&handle_connection, NULL, NULL));
	call_exec(equeue);
}

static void tr_run(int count)
{
	pthread_t threads[count];
	for (int i = 0; i < count; i++) {
		int *tr_n = malloc(sizeof(int));
		*tr_n = i;
		pthread_create(&threads[i], NULL, (void *)&tr_handler, tr_n);
	}
}

void run_server(int portno, http_handler _handler, logger _log_handler)
{
	struct sockaddr_in serv_addr;

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

	handler = _handler;
	log_handler = _log_handler;

	signal(SIGPIPE, &signal_callback_handler);
	listen(server_sockfd, 1024);

	int flags = fcntl(server_sockfd, F_GETFL, 0);
	fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK);

	tr_run(3);
	for(;;) sleep(2);
}

