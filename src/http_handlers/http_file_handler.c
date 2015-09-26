#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "../http_request.h"
#include "../http_response.h"
#include "../http_common.h"
#include "../lib/linked_list.h"

void get_local_path(char dest[], char reqPath[])
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		perror("getcwd() error");

	strcpy(dest, cwd);
	strcat(dest, "/");
	strcat(dest, reqPath);
	strcat(dest, "\0");
}

void write_file_to_res(int resdf, void *data)
{
	FILE *fp = fopen(data, "r");

	if (fp == NULL) {
		perror("Error while opening the file.\n");
		exit(1);
	}

	char ch;
	while ((ch = fgetc(fp)) != EOF)
		write(resdf, &ch, 1);

	fclose(fp);
}

void http_file_handler(http_request *req, http_response *res)
{
	char path[4096];
	get_local_path(path, req->path);

	if (access(path, F_OK) == -1) {
		res->status_code = 404;
		return;
	} else {
		res->status_code = 200;
	}

	res->body_data = &path;
	res->response_body_writer = write_file_to_res;

	struct stat st;
	stat(path, &st);

	char size_str[20];
	sprintf(size_str, "%jd", st.st_size);

	struct tm *tm = localtime(&st.st_mtime);
	char date_str[30];
	strftime(date_str, 30, "%a, %d %m %G %T %Z", tm);

	add_http_header(res, header_new("Content-Length", size_str));
	add_http_header(res, header_new("Last-Modified", date_str));
}