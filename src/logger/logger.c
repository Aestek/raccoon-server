#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../request.h"
#include "../response.h"

void log_raw(char msg[])
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char date_str[30];
	strftime(date_str, 30, "%c", tm);

	char line[2048] = "[";
	strcat(line, date_str);
	strcat(line, "] ");
	strcat(line, msg);

	// printf("%s\n", line);
}

void log_roundtrip(http_request *req, http_response *res)
{
	char msg[2048];
	sprintf(msg, "%s => %d", req->path, res->status_code);
	log_raw(msg);
}
