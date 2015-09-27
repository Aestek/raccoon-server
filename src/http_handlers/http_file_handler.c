#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include "http_file_handler.h"
#include "../request.h"
#include "../response.h"
#include "../header.h"
#include "../conf.h"
#include "../lib/linked_list.h"

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

path_status resolve_path_from_root(char root[], char resolved_path[], char req_path[])
{
	char absolute_path[REQUEST_PATH_MAX_LENGTH];
	strcpy(absolute_path, root);
	strcat(absolute_path, "/");
	strcat(absolute_path, req_path);
	strcat(absolute_path, "\0");

	if (realpath(absolute_path, resolved_path) == NULL) {
		switch (errno) {
			case EACCES:
				return PATH_EACCESS;
			case ENOENT:
				return PATH_ENOTFOUND;
			default:
				return PATH_ERESOLVE;
		}
	}

	int root_len = strlen(root);
	char *path_prefix = malloc(root_len * sizeof(char));
	memcpy(path_prefix, resolved_path, root_len);

	if (strcmp(root, path_prefix) != 0)
		return PATH_NOTINROOT;

	free(path_prefix);

	return PATH_OK;
}

void http_file_handler(http_request *req, http_response *res)
{
	char cwd[REQUEST_PATH_MAX_LENGTH];
	char path[REQUEST_PATH_MAX_LENGTH];

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		res->status_code = 500;
		return;
	}

	path_status status = resolve_path_from_root(cwd, path, req->path);

	switch (status) {
		case PATH_ENOTFOUND:
			res->status_code = 404;
			return;

		case PATH_NOTINROOT:
		case PATH_EACCESS:
			res->status_code = 403;
			return;

		case PATH_ERESOLVE:
			res->status_code = 500;
			return;

		default:
			res->status_code = 200;
	}

	res->body_data = &path;
	res->response_body_writer = &write_file_to_res;

	struct stat st;
	stat(path, &st);

	char size_str[20];
	sprintf(size_str, "%jd", st.st_size);

	struct tm *tm = localtime(&st.st_mtime);
	char date_str[30];
	strftime(date_str, 30, "%a, %d %m %G %T %Z", tm);

	list_append(&res->headers, header_new("Content-Length", size_str));
	list_append(&res->headers, header_new("Last-Modified", date_str));
}