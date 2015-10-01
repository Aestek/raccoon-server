#pragma once

#include "request.h"
#include "response.h"
#include "header.h"
#include "logger/logger.h"

typedef void (*http_handler)(http_request*, http_response*);

void run_server(int portno, http_handler, logger logger);