#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/client.h"
#include "src/commands/add.h"
#include "src/commands/commit.h"
#include "src/commands/create.h"
#include "src/commands/currentversion.h"
#include "src/commands/history.h"
#include "src/commands/push.h"
#include "src/commands/remove.h"
#include "src/commands/update.h"
#include "src/commands/destroy.h"
#include "src/configure.h"
#include "src/util/color.h"
#include "src/util/get_server_manifest.h"

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
      printf("Missing <project>\n");
      return 0;
    }
    create_client(argv[2]);

  } else if (strcmp(argv[1], "currentversion") == 0) {
    if (argc < 3) {
      printf("Missing <project name>\n");
      return 0;
    }
    currentversion_client(argv[2]);

  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 4) {
      printf("Missing <project name> and <filename>\n");
      return 0;
    }
    add_client(argv[2], argv[3]);

  } else if (strcmp(argv[1], "remove") == 0) {
    if (argc < 4) {
      printf("Missing <project name> and <filename>\n");
      return 0;
    }
    remove_client(argv[2], argv[3]);

  } else if (strcmp(argv[1], "commit") == 0) {
    if (argc < 3) {
      printf("Missing <project name>\n");
      return 0;
    }
    commit_client(argv[2]);

  } else if (strcmp(argv[1], "push") == 0) {
    if (argc < 3) {
      printf("Missing <project name>\n");
      return 0;
    }

    push_client(argv[2]);
  } else if (strcmp(argv[1], "update") == 0) {
    if (argc < 3) {
      printf("Missing <project name>\n");
      return 0;
    }

    update_client(argv[2]);
  } else if (strcmp(argv[1], "history") == 0) {
    if (argc < 3) {
      printf("Missing <project name>\n");
      return 0;
    }

    history_client(argv[2]);
  } else if (strcmp(argv[1], "destroy") == 0) {
    if (argc < 3) {
      printf("Missing <project name>\n");
      return 0;
    }

    destroy_client(argv[2]);
  } else {
    printf("Flag " BLU "%s" RESET " does not exist\n", argv[1]);
  }

  return 0;
}