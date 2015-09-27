#pragma once

#include "conf.h"

typedef struct {
	char name[HEADER_NAME_LEN];
	char value[HEADER_VALUE_LEN];
} http_header;

http_header* header_new(char name[], char value[]);
