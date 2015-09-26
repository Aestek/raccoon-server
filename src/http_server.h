#pragma once

#include "http_request.h"
#include "http_response.h"
#include "http_common.h"
#include "logger/logger.h"

void run_server(int portno, http_handler, logger logger);