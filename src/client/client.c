#include "client.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../color/color.h"
#include "../configure/configure.h"
#include "../request/request.h"
#include "../response/response.h"
#include "../token/token.h"

Response* client_send(Request* request) {
  int client_fd = client_open();

  request_write(client_fd, request);              // Write request
  Response* response = response_read(client_fd);  // Read response

  close(client_fd);
  return response;
}

int client_open() {
  Configure* config = configure_read();

  // `addrinfo` settings
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        // Use IPv4
  hints.ai_socktype = SOCK_STREAM;  // Use TCP

  // Convert URL/Port into IP
  struct addrinfo* result;
  if (getaddrinfo(config->ip, config->port, &hints, &result) != 0) {
    return -1;
  }

  // Create socket
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Add connection to it
  if (connect(socket_fd, result->ai_addr, result->ai_addrlen) != 0) {
    return -1;
  }

  return socket_fd;
}