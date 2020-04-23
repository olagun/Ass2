#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/server.h"

#ifndef CREATE_H
#define CREATE_H

void create_client(char* project_name);
Response* create_server(Request* request);

#endif