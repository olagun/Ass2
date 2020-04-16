#include <stdlib.h>

#include "token.h"

void configure(char* ip, char* port) {
    // Writes `port` to configure.
  Token* head = NULL;
  head = Token_append(head, Token_new(ip));
  head = Token_append(head, Token_new(" "));
  head = Token_append(head, Token_new(port));

  Token_write(".configure", head);
}