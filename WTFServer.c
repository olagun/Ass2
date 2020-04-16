#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "create.h"
#include "src/server.h"

void on_exit();
void on_interrupt();
char* on_message(char* message);

Server* server;

int main(int argc, char** argv) {
  atexit(on_exit);
  signal(SIGINT, on_interrupt);

  server = Server_create("8000");
  if (server == NULL) {
    return 0;
  }

  mkdir("projects", 0777);
  mkdir("history", 0777);
  mkdir("commits", 0777);

  while (1) {
    Server_accept(server, on_message);
  }

  return 0;
}

// <command>:<body>
char* on_message(char* message) {
  char* command = strtok(stdup(message), ":");
  char* body = strchr(message, ':') + 1;  // substring of messaage after <command>

  if (strcmp("create", command) == 0) {
    return Create_server(body);
  }

  // send message back
  return  "command not recognized";
}

void on_exit() { close(server->socket_fd); }

void on_interrupt() {
  printf("Bye\n");
  exit(1);
}