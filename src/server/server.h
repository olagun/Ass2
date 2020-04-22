#include "../request/request.h"
#include "../response/response.h"

#ifndef SERVER_H
#define SERVER_H

// Opens server on port
int server_open(char* port);

// Waits for new connections. Calls `on_accept` when recieves one
void server_wait(int server_fd, Response* (*on_accept)(Request*));

#endif