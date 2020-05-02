#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/commands/add.h"
#include "src/commands/checkout.h"
#include "src/commands/create.h"
#include "src/commands/remove.h"
#include "src/configure.h"
#include "src/util/color.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_size.h"

bool TESTING = true;

// add if (!TESTING) before print statements across the program

// HELPER FUNCTIONS
void print_header(char* command) {
  printf("\ntesting " BWHT "%s" RESET "...\n", command);
}

void assert(char* test, bool val) {
  if (val) {
    printf(GRN "passes '%s'\n" RESET, test);
  } else {
    printf(RED "fails '%s'\n" RESET, test);
  }
}
bool file_eq(char* a, char* b) {
  if (get_file_size(a) != get_file_size(b)) {
    return false;
  }

  // empty or doesn't exist
  if (get_file_size(a) == 0 && get_file_size(b) == 0) {
    return false;
  }

  int fd_a = open(a, O_RDONLY, 0777);
  int fd_b = open(a, O_RDONLY, 0777);

  int bytes_read_a = 0;
  int bytes_read_b = 0;

  char* buff_a = calloc(1, sizeof(char));
  char* buff_b = calloc(1, sizeof(char));

  while ((bytes_read_a = read(fd_a, buff_a, 1)) > 0 &&
         (bytes_read_b = read(fd_b, buff_b, 1)) > 0) {
    if (buff_a[0] != buff_b[0]) {
      close(fd_a);
      close(fd_b);
      return false;
    }
  }

  close(fd_a);
  close(fd_b);
  return true;
}

// SETUP
void setup_server() {
  mkdir("server", 0777);
  chdir("server");

  // Create folders to organize server
  mkdir("projects", 0777);  // Stores projects
  mkdir("history", 0777);   // Stores old projects
  mkdir("commits", 0777);   // Stores `.Commit` files

  chdir("../");
}
void setup_client() { mkdir("client", 0777); }
void clean_client() { system("rm -rf client"); }
void clean_server() { system("rm -rf server"); }

// RUN
Configure* run_configure() {
  chdir("client/test");

  configure_write("127.0.0.1", "8000");
  Configure* configure = configure_read();

  chdir("../../");

  return configure;
}
void run_create() {
  chdir("client");
  create_client("test");
  chdir("../");
}
void run_add() {
  system("cp answers/add/client/test/file.txt client/test");

  chdir("client");
  add_client("test", "file.txt");
  chdir("../");
}
void run_remove() {
  chdir("client");
  remove_client("test", "file.txt");
  chdir("../");
}
void run_checkout() {
  system("cp -R answers/checkout/server/projects/test server/projects");

  chdir("client");
  checkout_client("test");
  chdir("../");
}

// Configure
void test_configure() {
  print_header("configure");

  // Setup
  setup_client();
  setup_server();

  // Run
  run_create();
  Configure* config = run_configure();

  // Test
  assert("writes to .configure",
         file_eq("client/test/.configure",
                 "answers/configure/client/test/.configure"));
  assert("reads from .configure", strcmp(config->ip, "127.0.0.1") == 0 &&
                                      strcmp(config->port, "8000") == 0);

  // Cleanup
  remove(".configure");
  clean_server();
  clean_client();
}

// Create
void test_create() {
  print_header("create");

  // Setup
  setup_server();
  setup_client();

  // Run
  run_create();

  // Test
  assert("creates manifest on server",
         file_exists("server/projects/test/.Manifest"));
  assert("creates manifest on client", file_exists("client/test/.Manifest"));
  assert("correctly creates manifest on client",
         file_eq("client/test/.Manifest", "answers/create/test/.Manifest"));

  // Cleanup
  clean_server();
  clean_client();
}

// Add
void test_add() {
  print_header("add");

  // Setup
  setup_server();
  setup_client();

  // Run
  run_create();
  run_add();

  // Test
  assert("correctly adds file to manifest",
         file_eq("client/test/.Manifest", "answers/add/client/test/.Manifest"));

  // Cleanup
  clean_server();
  clean_client();
}

// Remove
void test_remove() {
  print_header("remove");

  // Setup
  setup_server();
  setup_client();

  // Run
  run_create();
  run_add();
  run_remove();

  // Test
  assert(
      "correctly removes file from client",
      file_eq("client/test/.Manifest", "answers/remove/client/test/.Manifest"));

  // Cleanup
  clean_server();
  clean_client();
}

// Checkout
void test_checkout() {
  print_header("checkout");

  // Setup
  setup_server();
  setup_client();

  // Run
  run_create();
  run_configure();
  run_checkout();

  // Test
  assert("correctly checks out manifest",
         file_eq("client/test/.Manifest", "server/projects/test/.Manifest"));
  assert("correctly checks out file",
         file_eq("client/test/file.txt", "server/projects/test/file.txt"));

  // Cleanup
  clean_server();
  clean_client();
}

// Current Version
// test after doing push/commit
void test_currentversion() {
  // // Setup
  // setup_server();
  // setup_client();

  // // Run
  // run_create();
  // run_configure();
  // run_checkout();

  // // Test
  // assert("correctly checks out manifest",
  //        file_eq("client/test/.Manifest", "server/projects/test/.Manifest"));
  // assert("correctly checks out file",
  //        file_eq("client/test/file.txt", "server/projects/test/file.txt"));

  // // Cleanup
  // clean_server();
  // clean_client();
}

void test_history() {}

void test_rollback() {}

void test_commit() {}

void test_push() {}

void test_update() {}

void test_upgrade() {}

void test_destroy() {}

int main(int argc, char** argv) {
  test_configure();
  test_create();
  test_add();
  test_remove();
  test_checkout();

  return 0;
}
