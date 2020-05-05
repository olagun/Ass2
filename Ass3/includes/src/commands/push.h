#include "src/response.h"
#include "src/request.h"

#ifndef PUSH_H
#define PUSH_H

void push_client(char* project_name);
Response* push_server(Request* request);

#endif