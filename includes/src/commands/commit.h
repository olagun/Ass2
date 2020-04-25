#include "src/request.h"
#include "src/response.h"

#ifndef COMMIT_H
#define COMMIT_H

void commit_client(char* project_name);
Response* commit_server(Request* request);

#endif