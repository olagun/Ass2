#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/request.h"
#include "src/response.h"
#include "src/server.h"

#ifndef ROLLBACK_H
#define ROLLBACK_H

void rollback_client(char* project_name, char* project_version);
Response* rollback_server(Request* request);

#endif