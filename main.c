#include <stdio.h>
#include <stdlib.h>
#include "src/server.h"
#include "src/http_handlers/http_file_handler.h"
#include "src/logger/logger.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	int portno = atoi(argv[1]);

	printf("Starting server on port %d...\n", portno);
	run_server(portno, &http_file_handler, &log_roundtrip);

	return 0;
}