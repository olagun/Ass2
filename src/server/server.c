#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../color/color.h"
#include "../read/read.h"
#include "../request/request.h"
#include "../response/response.h"
#include "../token/token.h"

// TODO: Open new thread after accepting connection
void server_wait(int server_fd, Response* (*on_accept)(Request*)) {
  int client_fd = accept(server_fd, NULL, NULL);

  printf("\n");
  printf(BWHT "Accepted new connection" RESET "\n");

  Request* request = request_read(client_fd);     // Read request
  response_write(client_fd, on_accept(request));  // Write response

  close(client_fd);

  // create a new thread
  // pthread_t thread;
  // pthread_create(&thread, NULL, on_accept, request);
}

int server_open(char* port) {
  // `addrinfo` settings
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Convert URL/Port to IP
  struct addrinfo* result;
  if (getaddrinfo(NULL, port, &hints, &result) != 0) {
    printf("%s\n", strerror(errno));
    return -1;
  }

  // Create and connect socket
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int option = 1;
  // https://stackoverflow.com/questions/5106674/error-address-already-in-use-while-binding-socket-with-address-but-the-port-num
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  if (bind(socket_fd, result->ai_addr, result->ai_addrlen) != 0) {
    printf("%s\n", strerror(errno));
    return -1;
  }

  // Listen for incoming connections
  if (listen(socket_fd, 128) != 0) {
    printf("%s\n", strerror(errno));
    return -1;
  }

  printf("Server running on " GRN ":%s" RESET "\n", port);

  return socket_fd;
}
