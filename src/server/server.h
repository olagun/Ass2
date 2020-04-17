#include "../token/token.h"

#ifndef SERVER_H
#define SERVER_H

// Container for the port number
// and the socket file descriptor.
typedef struct {
  int socket_fd;
  char* port;
} Server;

typedef char* Handler(char* command, char* body);

void Server_accept(Server* server, Handler* handler);
Server* Server_create(char* port);

#endif