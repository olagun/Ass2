#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/server.h"

char* handle_message(Token* message) {
  // send message back
  return "some message in response";
}

int main(int argc, char** argv) {
  Server* server = Server_create("8000");

  while (1) {
    Server_accept(server, handle_message);
  }

  return 0;
}
