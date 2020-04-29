#ifndef COMMIT_H
#define COMMIT_H

#include "src/request.h"
#include "src/response.h"

void commit_client(char* project_name);
Response* commit_server(Request* request);

#endif