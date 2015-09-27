#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "conf.h"
#include "lib/linked_list.h"

http_header* header_new(char name[], char value[])
{
	http_header *nh = malloc(sizeof(http_header));
	strncpy(nh->name, name, HEADER_NAME_LEN);
	strncpy(nh->value, value, HEADER_VALUE_LEN);

	return nh;
}