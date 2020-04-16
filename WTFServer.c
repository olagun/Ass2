#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

  while (1) {
    Server_accept(server, on_message);
  }

  return 0;
}

char* on_message(char* message) {
  // send message back
  return "some message in response";
}

void on_exit() { close(server->socket_fd); }

void on_interrupt() {
  printf("Bye\n");
  exit(1);
}