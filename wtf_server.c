#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/accept.h"
#include "src/server.h"

int server_fd;

// Closes server
void on_exit() { close(server_fd); }

// Catches `Ctrl + C`
void on_interrupt() {
  // Clear memory here?
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
  mkdir("projects", 0777);  // Stores projects
  mkdir("history", 0777);   // Stores old projects
  mkdir("commits", 0777);   // Stores `.Commit` files

  // Wait for and accept new connections forever
  while (1) {
    server_wait(server_fd, accept);
  }

  return 0;
}
