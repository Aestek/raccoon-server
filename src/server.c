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
#include "server.h"
#include "header.h"
#include "request.h"
#include "lib/linked_list.h"
#include "lib/array_queue.h"

sem_t queue_len_mutex;
sem_t queue_free_mutex;
pthread_mutex_t queue_pop_mutex;
array_queue queue;
http_handler handler;
logger log_handler;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void signal_callback_handler(int signum)
{
	printf("Caught signal SIGPIPE %d\n",signum);
}

static void handle_connection(int client_sockfd)
{
	http_request *req = http_request_new();
	http_response *res = http_response_new();
	strcpy(res->http_version_str, "HTTP/1.1");

	if (http_request_parse(req, client_sockfd) != 1) {
		handler(req, res);
	} else {
		res->status_code = 400;
	}

	http_response_write(req, res);
	log_handler(req, res);
	free(req);
	http_response_destroy(res);
}

static void tr_handler(void *ptr)
{
	int *sock_fd;
	for (;;) {
		sem_wait(&queue_len_mutex);
		pthread_mutex_lock(&queue_pop_mutex);
		sock_fd = array_queue_pop(&queue);
		pthread_mutex_unlock(&queue_pop_mutex);
		sem_post(&queue_free_mutex);

		handle_connection(*sock_fd);
		close(*sock_fd);
		free(sock_fd);
	}
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
	int server_sockfd;
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

	int queue_size = 1024;
	sem_init(&queue_free_mutex, 0, queue_size);
	sem_init(&queue_len_mutex, 0, 0);
	pthread_mutex_init(&queue_pop_mutex, NULL);
	array_queue_init(&queue, 2048);
	handler = _handler;
	log_handler = _log_handler;

	tr_run(4);
	signal(SIGPIPE, &signal_callback_handler);

	listen(server_sockfd, TCP_BACKLOG_SIZE);

	client_addr_length = sizeof(cli_addr);

	for (;;) {
		sem_wait(&queue_free_mutex);

		int *client_sockfd = malloc(sizeof(int));
		*client_sockfd = accept(
			server_sockfd,
			(struct sockaddr *) &cli_addr,
			&client_addr_length
		);

		if (*client_sockfd < 0)
			error("ERROR on accept");

		array_queue_push(&queue, client_sockfd);
		sem_post(&queue_len_mutex);
	}

	close(server_sockfd);
}

