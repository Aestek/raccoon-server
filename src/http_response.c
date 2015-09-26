#include <stdio.h>
#include <stdlib.h>
#include "http_response.h"

http_response* http_response_new()
{
	http_response *res = malloc(sizeof(http_response));
	list_new(&res->headers, NULL);
	return res;
}
