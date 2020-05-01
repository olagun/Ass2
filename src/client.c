#include "src/client.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/configure.h"
#include "src/request.h"
#include "src/response.h"
#include "src/token.h"
#include "src/util/color.h"

Response* client_send(Request* request) {
  int client_fd = client_open();
  if (client_fd < 0) {
    printf("[Client Error] Could not open a client connection.\n");
    exit(1);
    return NULL;
  }

  request_write(client_fd, request);              // Write request
  Response* response = response_read(client_fd);  // Read response

  close(client_fd);
  return response;
}

int client_open() {
  Configure* config = configure_read();

  if (config == NULL || strlen(config->ip) == 0 || strlen(config->port) == 0) {
    printf("[Configure Error] Could not read .configure.");
    return -1;
  }

  // `addrinfo` settings
  struct addrinfo hints = {0};
  hints.ai_family = AF_INET;        // Use IPv4
  hints.ai_socktype = SOCK_STREAM;  // Use TCP

  // Convert URL/Port into IP
  struct addrinfo* result;
  if (getaddrinfo(config->ip, config->port, &hints, &result) != 0) return -1;

  // Create socket
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Add connection to socket
  if (connect(socket_fd, result->ai_addr, result->ai_addrlen) != 0) return -1;

  return socket_fd;
}