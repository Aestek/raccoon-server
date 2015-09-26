#pragma once

#include "../http_response.h"

void get_local_path(char dest[], char reqPath[]);

void write_file_to_res(int resdf, char path[]);

void http_file_handler(http_request *req, http_response *res);