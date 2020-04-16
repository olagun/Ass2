#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/client.h"
#include "src/color.h"
#include "src/configure.h"

int main(int argc, char** argv) {
  Token* response = Client_send("127.0.0.1", "8000", "some message");

  // TODO: Refactor into 'flags.c'
  if (argc < 2) {
    printf("Too few arguments\n");
    return 0;
  }

  if (strcmp(argv[1], "configure") == 0) {
    if (argc < 4) {
      printf("Missing <ip/hostname> and/or <port>\n");
      return 0;
    }

    configure(argv[2], argv[3]);
  } else {
    printf("Flag '" BLU "%s" RESET "' does not exist\n", argv[1]);
  }

  return 0;
}