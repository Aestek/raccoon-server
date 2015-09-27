#pragma once

#include "../response.h"

typedef enum {
	PATH_OK,
	PATH_ENOTFOUND,
	PATH_NOTINROOT,
	PATH_EACCESS,
	PATH_ERESOLVE
} path_status;

void get_local_path(char dest[], char reqPath[]);

void write_file_to_res(int resdf, void *data);

void http_file_handler(http_request *req, http_response *res);