#include "src/request.h"
#include "src/response.h"

#ifndef SERVER_H
#define SERVER_H

// Opens server on port
int server_open(char* port);

// Waits for new connections. Calls `on_connection` when recieves one
void server_wait(int server_fd, void* (*on_connection)(void*));

#endif