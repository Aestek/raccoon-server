#include "http_response.h"
#include "http_request.h"
#include "http_common.h"

void write_crlf(http_request *req);

void write_header(http_request *req, http_header *header);

void write_response(http_request *req, http_response *res);