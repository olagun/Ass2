#include "server.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "token.h"

#define BLU "\e[0;34m"
#define GRN "\e[0;32m"
#define RESET "\e[0m"

void Server_accept(Server* server, Handler* handler) {
  int client_fd = accept(server->socket_fd, NULL, NULL);
  printf(BLU "1" RESET " new connection on " GRN ":%s\n" RESET, server->port);

  // char* buffer = malloc(sizeof(char) * 2);
  // memset(buffer, '\0', 2);
  // Token* message = NULL;
  // while (read(client_fd, buffer, 1) > 0) {
  //   Token* token = Token_new(buffer);
  //   message = Token_append(message, token);
  // }

  char* response = handler(NULL);
  write(client_fd, response, strlen(response));

  close(client_fd);
}

Server* Server_create(char* port) {
  // `addrinfo` settings.
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Convert URL/Port into IP.
  if (getaddrinfo(NULL, "8000", &hints, &result) != 0) {
    return NULL;
  }

  // Create socket and connect to port.
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (bind(socket_fd, result->ai_addr, result->ai_addrlen) != 0) {
    return NULL;
  }

  // Listen for incoming connections.
  if (listen(socket_fd, 128) != 0) {
    return NULL;
  }

  printf("Listening on " GRN ":%s" RESET "\n", port);

  Server* server = malloc(sizeof(Server));
  server->socket_fd = socket_fd;
  server->port = port;
  return server;
}
