#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "src/accept.h"
#include "src/mutexlist.h"
#include "src/request.h"
#include "src/response.h"
#include "src/server.h"
#include "src/testing.h"
#include "src/util/color.h"

bool TESTING = false;

int server_fd;

void on_leave() {
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

// Thread that runs on every new connection
// See: `server_wait`
void* on_connection(void* client_fd_ptr) {
  int client_fd = *((int*)client_fd_ptr);

  Request* request = request_read(client_fd);  // Read request

  char* command_name = request->command_name;
  char* project_name = request->project_name;

  bool includes_project = strlen(project_name) > 0;
  bool is_create = strcmp(command_name, "create") == 0;
  bool is_destroy = strcmp(command_name, "destroy") == 0;

  // Lock if request includes a project name
  // Don't lock "create" or "destroy" because they add and remove mutexes
  bool should_lock = includes_project && !is_create && !is_destroy;

  // [Lock] Mutex
  if (should_lock) lock_project(project_name);

  response_write(client_fd, on_accept(request));  // Write response

  // [Unlock] Mutex
  if (should_lock) unlock_project(project_name);

  close(client_fd);
  return NULL;
}

int main(int argc, char** argv) {
  atexit(on_leave);
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
    server_wait(server_fd, on_connection);
  }

  return 0;
}
