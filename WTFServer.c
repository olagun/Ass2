#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "commands/create/create.h"
#include "src/server/server.h"

Server* server;

// Closes the server socket before exiting.
void on_exit() { close(server->socket_fd); }

// Catches `Ctrl + C` interrupt.
// May be used later on when deallocating the closed program.
void on_interrupt() {
  printf("Bye\n");
  exit(1);
}

// Function called every time a message is recieved.
char* on_request(char* command, char* body);

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Missing arguments.\n");
    return 0;
  }

  // Register signal handlers.
  atexit(on_exit);
  signal(SIGINT, on_interrupt);

  // Start server on port.
  char* port = argv[1];
  server = Server_create(port);
  if (server == NULL) {
    printf("Server couldn't be started.\n");
    return 0;
  }

  // Setup server folders.
  mkdir("projects", 0777);
  mkdir("history", 0777);
  mkdir("commits", 0777);

  // Start accepting client connections.
  while (1) {
    Server_accept(server, on_request);
  }

  return 0;
}

// Takes in a client request command and body.
// Returns response as a string.
char* on_request(char* command, char* body) {
  if (strcmp("create", command) == 0) {
    return Create_server(body);
  }

  // if (strcmp("<command_name>", command) == 0) {
  //   return <Command_name>_server(body);
  // }

  return "Command not recognized.";
}
