#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/accept.h"
#include "src/server.h"
#include "src/testing.h"
#include "src/util/color.h"

bool TESTING = false;

int server_fd;

void on_exit() {
  printf(BYEL "[Exit]" RESET " Closing server socket\n");
  close(server_fd);
  // printf(BYEL "[Exit]" RESET " Shutting down all threads\n");
  printf("Bye\n");
}

// Catches `Ctrl + C`
void on_interrupt() {
  printf(BYEL "[Signal]" RESET " Caught interrupt signal\n");
  exit(1);
}

int main(int argc, char** argv) {
  atexit(on_exit);
  signal(SIGINT, on_interrupt);

  if (argc != 2) {
    printf(BWHT "[Flag Error]" RESET " Invalid number of arguments\n");
    return 0;
  }

  int server_fd = server_open(argv[1]);
  if (server_fd < 0) {
    printf(BRED "[Servor Error]" RESET " Server couldn't start\n");
    return 0;
  }

  mkdir("projects", 0777);
  mkdir("history", 0777);
  mkdir("commits", 0777);

  // Wait for and accept new connections
  while (true) {
    server_wait(server_fd, on_accept);
  }

  return 0;
}
