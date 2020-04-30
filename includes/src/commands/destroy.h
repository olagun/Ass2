#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/request.h"
#include "src/response.h"
#include "src/server.h"

#ifndef DESTROY_H
#define DESTROY_H

void destroy_client(char* project_name);
Response* destroy_server(Request* request);

#endif