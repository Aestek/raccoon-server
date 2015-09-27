#pragma once

#include "../request.h"
#include "../response.h"

typedef void (*logger)(http_request *, http_response*);

void log_raw(char msg[]);

void log_roundtrip(http_request *req, http_response *res);