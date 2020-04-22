#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../src/server/server.h"

#ifndef CURRENT_VERSION_H
#define CURRENT_VERSION_H

void currentversion_client(char* project_name);
Response* currentversion_server(Request* request);

#endif