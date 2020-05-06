#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/accept.h"
#include "src/request.h"
#include "src/response.h"
#include "src/token.h"

#ifndef CLIENT_H
#define CLIENT_H

int client_open();
Response* client_send(Request* request);

#endif