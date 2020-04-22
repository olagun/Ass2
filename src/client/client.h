#include "../request/request.h"
#include "../response/response.h"
#include "../token/token.h"

#ifndef CLIENT_H
#define CLIENT_H

int client_open();
Response* client_send(Request* request);

#endif