#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "http_common.h"
#include "lib/linked_list.h"
#include "http_response.h"

void add_http_header(http_response *response, http_header *header)
{
	list_append(&response->headers, header);
}

http_header* header_new(char name[], char value[])
{
	http_header *nh = malloc(sizeof(http_header));
	strcpy(nh->name, name);
	strcpy(nh->value, value);

	return nh;
}