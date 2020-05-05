#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/server.h"

#ifndef HISTORY_H
#define HISTORY_H

void history_client(char* project_name);
Response* history_server(Request* request);

#endif