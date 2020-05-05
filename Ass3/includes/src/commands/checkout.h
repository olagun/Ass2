#ifndef CHECKOUT_H
#define CHECKOUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/request.h"
#include "src/response.h"
#include "src/server.h"

void checkout_client(char* project_name);
//void checkout_server(Request *request);

#endif