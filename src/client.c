#include "src/client.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "src/accept.h"
#include "src/configure.h"
#include "src/request.h"
#include "src/response.h"
#include "src/testing.h"
#include "src/token.h"
#include "src/util/color.h"

Response* client_send(Request* request) {
  if (TESTING) {
    chdir("../server");
    Response* response = on_accept(request);
    chdir("../client");
    return response;
  }

  // "Client processes that cannot find the server should repeatedly try to
  // connect every 3 seconds until killed"
  int client_fd;
  while ((client_fd = client_open()) < 0) {
    printf(BRED "[Client Error]" RESET " Couldn't connect. Retrying ");

    const char* dots[] = {".\b", "..\b\b", "...\b\b\b"};
    int i = 0;
    for (; i < 3; i++) {
      printf("%s", dots[i]);
      fflush(stdout);
      sleep(1);
    }

    printf("\n");
  }
  printf("\n");

  // "Client announces completion of connection to server"
  Configure* config = configure_read();
  printf(BGRN "[Client Success]" RESET " Connected to server [at] %s" GRN
              ":%s\n" RESET,
         config->ip, config->port);

  request_write(client_fd, request);              // Write request
  Response* response = response_read(client_fd);  // Read response

  close(client_fd);
  return response;
}

int client_open() {
  Configure* config = configure_read();

  if (config == NULL || strlen(config->ip) == 0 || strlen(config->port) == 0) {
    printf(BRED "[Configure Error]" RESET
                " Could not read from `.configure`\n" RESET);
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