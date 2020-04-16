#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "color.h"
#include "token.h"

// TODO: A new thread needs to opened on each new connection
void Server_accept(Server* server, Handler* handler) {
  int client_fd = accept(server->socket_fd, NULL, NULL);

  // Read client messsage.
  char* buffer = malloc(sizeof(char) * 2);
  memset(buffer, '\0', 2);
  Token* message = NULL;
  while (read(client_fd, buffer, 1) > 0) {
    if (*buffer == '\0') break;
    Token* token = Token_new(buffer);
    message = Token_append(message, token);
  }
  char* message_string = Token_to_string(message);
  printf("Recieved '" BLU "%s" RESET "' from client\n", message_string);

  // Create and send response.
  char* response = handler(message_string);
  write(client_fd, response, strlen(response));
  printf("Sent '" BLU "%s" RESET "' to the client\n", response);

  close(client_fd);
}

Server* Server_create(char* port) {
  // `addrinfo` settings.
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Convert URL/Port into IP.
  struct addrinfo* result;
  if (getaddrinfo(NULL, port, &hints, &result) != 0) {
    printf("%s\n", strerror(errno));
    return NULL;
  }

  // Create socket and connect to port.
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int option = 1;
  // https://stackoverflow.com/questions/5106674/error-address-already-in-use-while-binding-socket-with-address-but-the-port-num
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  if (bind(socket_fd, result->ai_addr, result->ai_addrlen) != 0) {
    printf("%s\n", strerror(errno));
    return NULL;
  }

  // Listen for incoming connections.
  if (listen(socket_fd, 128) != 0) {
    printf("%s\n", strerror(errno));
    return NULL;
  }
  printf("Listening on " GRN ":%s" RESET "\n", port);

  Server* server = malloc(sizeof(Server));
  server->socket_fd = socket_fd;
  server->port = port;
  return server;
}
