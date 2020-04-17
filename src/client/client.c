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
#include "../token/token.h"

// Returns a linked list containing the response.
char* Client_send(char* message) {
  Configure* config = Configure_read();

  // `addrinfo` settings.
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        // Use IPv4.
  hints.ai_socktype = SOCK_STREAM;  // Use TCP.

  // Convert URL/Port into IP.
  struct addrinfo* result;
  if (getaddrinfo(config->ip, config->port, &hints, &result) != 0) {
    // Could not find server.
    return NULL;
  }

  // Create socket to add connection to it.
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(socket_fd, result->ai_addr, result->ai_addrlen) != 0) {
    // Could not connect to server.
    return NULL;
  }

  // Send message.
  write(socket_fd, message, strlen(message) + 1);  // - write -
  printf("Sent '" BLU "%s" RESET "' to the server\n", message);

  // Reads response into linked list.
  char* buffer = malloc(sizeof(char) * 2);
  memset(buffer, '\0', 2);
  Token* head = NULL;
  while (read(socket_fd, buffer, 1) > 0) {  // - read -
    Token* token = Token_new(buffer);
    head = Token_append(head, token);
  }
  close(socket_fd);
  char* response_string = Token_to_string(head);
  printf("Recieved '" BLU "%s" RESET "' in response\n", response_string);

  return Token_to_string(head);
}
