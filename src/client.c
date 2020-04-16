#include "client.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BLU "\e[0;34m"
#define GRN "\e[0;32m"
#define RESET "\e[0m"

// Returns a linked list containing the response.
Token* Client_send(char* url, char* port, char* message) {
  // `addrinfo` settings.
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        // Use IPv4.
  hints.ai_socktype = SOCK_STREAM;  // Use TCP.

  // Convert URL/Port into IP.
  if (getaddrinfo(url, port, &hints, &result) != 0) {
    // Could not find server.
    return NULL;
  }

  // Create socket to add connection to.
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  // Connect socket to address.
  if (connect(fd, result->ai_addr, result->ai_addrlen) != 0) {
    // Could not connect to server.
    return NULL;
  }

  printf("Sent '" BLU "%s" RESET "' to the server\n", message);
  // Write message to socket connection.
  write(fd, message, strlen(message));

  // Read one char at a time in to linked l ist.
  char* buffer = malloc(sizeof(char) * 2);
  memset(buffer, '\0', 2);
  Token* head = NULL;
  while (read(fd, buffer, 1) > 0) {
    Token* token = Token_new(buffer);
    head = Token_append(head, token);
  }
  close(fd);

  return head;
}
