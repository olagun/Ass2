#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../src/server/server.h"

#ifndef CREATE_H
#define CREATE_H

void Create_client(char* project_name);
char* Create_server(char* project_name);

#endif