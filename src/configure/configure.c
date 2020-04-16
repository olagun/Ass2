#include "configure.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../token/token.h"

void Configure_write(char* ip, char* port) {
  // Writes `port` to configure.
  Token* head = NULL;
  head = Token_append(head, Token_new(ip));
  head = Token_append(head, Token_new(":"));
  head = Token_append(head, Token_new(port));

  Token_write(".configure", head);
}

// TODO: check if file is null or token_read is null, etc.
// Reads in the configuration file.
Configure* Configure_read() {
  char* file = Token_to_string(Token_read("./.configure"));

  char* ip = strtok(strdup(file), ":");
  char* port = strchr(file, ':') + 1;

  Configure* config = malloc(sizeof(Configure));
  config->ip = ip;
  config->port = port;
  return config;
}