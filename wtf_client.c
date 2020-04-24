#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/client.h"
#include "src/commands/add.h"
#include "src/commands/create.h"
#include "src/commands/currentversion.h"
#include "src/configure.h"
#include "src/util/color.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Too few arguments\n");
    return 0;
  }

  if (strcmp(argv[1], "configure") == 0) {
    if (argc < 4) {
      printf("Missing <ip/hostname> and/or <port>\n");
      return 0;
    }

    configure_write(argv[2], argv[3]);
  } else if (strcmp(argv[1], "create") == 0) {
    if (argc < 3) {
      printf("Missing <project>");
      return 0;
    }

    create_client(argv[2]);
  } else if (strcmp(argv[1], "currentversion") == 0) {
    if (argc < 3) {
      printf("Missing <project version>");
      return 0;
    }

    currentversion_client(argv[2]);
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 4) {
      printf("Missing <project name> and <filename>");
      return 0;
    }

    add_client(argv[2], argv[3]);
  } else {
    printf("Flag " BLU "%s" RESET " does not exist\n", argv[1]);
  }

  return 0;
}