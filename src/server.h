#include "token.h"

#ifndef SERVER_H
#define SERVER_H

typedef char* Handler(Token*);

typedef struct Server {
  int socket_fd;
  char* port;
} Server;

void Server_accept(Server* server, Handler* handler);
Server* Server_create(char* port);

#endif