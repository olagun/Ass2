#include <stdio.h>
#include <stdlib.h>

#include "src/client.h"

int main(int argc, char** argv) {
  Token* response = Client_send("127.0.0.1", "8000", "some message");

  if (response != NULL) {
    Token_print(response);
  }

  return 0;
}