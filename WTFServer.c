#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "commands/create/create.h"
#include "commands/currentversion/currentversion.h"
#include "src/server/server.h"

int server_fd;
Response* on_accept(Request* request);

// Closes server
void on_exit() { close(server_fd); }

// Catches `Ctrl + C`
void on_interrupt() {
  // TODO: clear memory here?
  printf("Bye\n");
  exit(1);
}

int main(int argc, char** argv) {
  // Setup signal handlers
  atexit(on_exit);
  signal(SIGINT, on_interrupt);

  // Flags
  if (argc < 2) {
    printf("Missing arguments.\n");
    return 0;
  }

  // Open server
  int server_fd = server_open(argv[1]);
  if (server_fd < 0) {
    printf("Server couldn't be started.\n");
    return 0;
  }

  // Create folders to organize server
  mkdir("projects", 0777);  // Stores project
  mkdir("history", 0777);   // Stores old projects
  mkdir("commits", 0777);   // Stores `.Commit` files

  // Wait and accept new connections forever
  while (1) {
    server_wait(server_fd, on_accept);
  }

  return 0;
}

// Accepts and responds to requests
Response* on_accept(Request* request) {
  char* command_name = request->command_name;

  if (strcmp("create", command_name) == 0) {
    return create_server(request);
  }

  if (strcmp("currentversion", command_name) == 0) {
    return currentversion_server(request);
  }

  // if (strcmp("<command_name>", command) == 0) {
  //   return <Command_name>_server(body);
  // }

  return NULL;
}
