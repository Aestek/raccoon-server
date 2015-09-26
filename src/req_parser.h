#pragma once

#include "http_request.h"

void parse_payload(http_request *req, char payload[], int payload_size);

http_request *parse_request(int client_sockfd);