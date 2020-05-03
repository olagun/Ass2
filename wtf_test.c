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
#include "src/commands/commit.h"
#include "src/commands/remove.h"
#include "src/configure.h"
#include "src/util/color.h"
#include "src/util/file_exists.h"
#include "src/util/directory_exists.h"
#include "src/util/get_file_size.h"
#include "src/util/get_file_hash.h"

bool TESTING = true;

// add if (!TESTING) before print statements across the program

// HELPER FUNCTIONS
int total_num_tests = 0;
int total_tests_passed = 0;

int tests_passed = 0;
int num_tests = 0;

void start_tests() {
  tests_passed = 0;
  num_tests = 0;
}

void end_tests() { total_tests_passed += tests_passed; }

void print_result() { printf("%d/%d tests passed\n", tests_passed, num_tests); }

// void print_total_result() { printf("%d/%d tests passed\n", tests_passed, num_tests); }

void print_header(char* command) {
  printf("\ntesting " BWHT "%s" RESET "...\n", command);
}

void assert(char* test, bool val) {
  num_tests++;

  if (val) {
    printf(GRN "(%d) passes '%s'\n" RESET, num_tests, test);
    tests_passed++;
  } else {
    printf(RED "(%d) fails '%s'\n" RESET, num_tests, test);
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
Configure* run_configure(char* proj_name) {
  char* sys_cd = calloc(strlen(proj_name) + 50, sizeof(char));
  sprintf(sys_cd, "client/%s", proj_name);
  system(sys_cd);

  chdir(sys_cd);

  configure_write("127.0.0.1", "8000");
  Configure* configure = configure_read();

  chdir("../../");

  return configure;
}
void run_create(char* proj_name) {
  chdir("client");
  create_client(proj_name);
  chdir("../");
}
void run_add(char* proj_name, char* file_name) {
  char* sys_cp = calloc(strlen(proj_name) + 50, sizeof(char));
  sprintf(sys_cp, "cp answers/files/%s client/%s", file_name, proj_name);
  system(sys_cp);

  chdir("client");
  add_client(proj_name, file_name);
  chdir("../");
}
void run_remove(char* proj_name, char* file_name) {
  chdir("client");
  remove_client(proj_name, file_name);
  chdir("../");
}

void run_checkout(char* proj_name) {
  char* sys_cp = calloc(strlen(proj_name) + 50, sizeof(char));
  sprintf(sys_cp, "cp -R answers/checkout_example server/projects/%s",proj_name);
  system(sys_cp);

  chdir("client");
  checkout_client(proj_name);
  chdir("../");
}

// Configure
void test_configure() {
  print_header("configure");

  // Setup
  setup_client();
  setup_server();

  // Run
  run_create("test");
  Configure* config = run_configure("test");

  // Test
  start_tests();
  assert("writes to .configure",
         file_exists("client/test/.configure"));
  assert("reads from .configure", strcmp(config->ip, "127.0.0.1") == 0 &&
                                      strcmp(config->port, "8000") == 0);
  end_tests();
  print_result();

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
  run_create("test");

  // Test
  start_tests();

  assert("creates manifest on server",
         file_exists("server/projects/test/.Manifest"));

  assert("creates manifest on client", file_exists("client/test/.Manifest"));

  chdir("client");
  Manifest* manifest = manifest_read("test");
  chdir("../");
  assert("manifest version is 0",
        manifest->project_version == 0);

  end_tests();
  print_result();

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
  run_create("test");
  run_add("test", "example1.txt");

  // Test
  start_tests();

  chdir("client");
  Manifest* manifest = manifest_read("test");
  chdir("../");
  assert("adds one file to manifest",
        filelist_size(manifest->filelist) == 1);

  // assert("adds one file to manifest",
  //      manifest->filelist && manifest->filelist->file_version == 0);

  end_tests();
  print_result();

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
  run_create("test");
  run_add("test", "example1.txt");
  run_remove("test", "example1.txt");

  // Test
  start_tests();

  // assert(
  //     "correctly removes file from client",
  //     file_eq("client/test/.Manifest", "answers/remove/client/test/.Manifest"));
  end_tests();
  print_result();

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
  run_create("test");
  run_configure("test");
  run_checkout("test");

  // Test
  start_tests();
  assert("correctly checks out manifest",
         file_eq("client/test/.Manifest", "server/projects/test/.Manifest"));
  assert("correctly checks out file",
         file_eq("client/test/file.txt", "server/projects/test/file.txt"));
  end_tests();
  print_result();

  // Cleanup
  clean_server();
  clean_client();
}

// Current Version
// test after doing push/commit
void test_currentversion() {
  print_header("currentversion");

  // // Setup
  // setup_server();
  // setup_client();

  // // Run
  // chdir("client");
  // Request* request = request_new();
  // request->command_name = "currentversion";
  // request->project_name = "test";
  // Response* response = on_accept(request);

  // FileList* files = response->filelist;
  
  // Test
  start_tests();
  // assert("prints version 0 after create",
  //        file_eq("client/test/.Manifest", "answers/currentversion/after_create/.Manifest"));
  end_tests();
  print_result();

  // Cleanup
  // clean_server();
  // clean_client();
}

void run_commit(char* proj_name) {
  chdir("client");
  commit_client(proj_name);
  chdir("../");
}

// test regular push
// test every scenario where upgrade is needed
void test_commit() {
  not_on_server: { 
    // Setup
    setup_server();
    setup_client();

    // Run
    run_commit("test");

    // Tests
    assert("[add case] doesn't commit if the project doesn't exist on the server",
          directory_exists("server/commits/test") == false);

    // Cleanup
    clean_server();
    clean_client();
  }

  add_case: { 
    // Setup
    setup_server();
    setup_client();

    // Run
    run_create("test");
    run_add("test", "example1.txt");
    run_commit("test");
   
    // Tests
    // assert("[add case] adds correct `.Commit` file to server",
    //       file_eq(
    //         "server/commits/test/.Commit_39675dfa2511a96c7cce0c45d0347802dbe5fcdaf0794f5cd21cc69084d260c1", 
    //         "answers/commit/add_case/.Commit"));
    
    // Cleanup
    clean_server();
    clean_client();
  }

  modify_case: {
    // Setup
    setup_server();
    setup_client();

    // // Run
    // run_create();
    // run_add();
    // run_commit();

    // // Modify file
    // system("cp answers/commit/modify_case/file.txt client/test/file.txt")

    // run_add();
    // run_commit();
   
    // // Tests
    // assert("[add case] adds correct `.Commit` file to server",
    //       file_eq(
    //         "server/commits/test/.Commit_39675dfa2511a96c7cce0c45d0347802dbe5fcdaf0794f5cd21cc69084d260c1", 
    //         "answers/commit/add_case/.Commit"));
    
    // Cleanup
    clean_server();
    clean_client();
  }

  
  delete_case: {}

  

}

void test_push() {

   //  chdir("client");
  //  Manifest* manifest= manifest_read("test");
   // chdir("../");
    // These should be in push no commit. The assignment description must be wrong.
    // otherwise every single commit would increment the version number not every push.
    // assert("[add case] increments client manifest project version number",
    //       manifest->project_version == 1);

    // assert("[add case] increments client manifest file version number",
    //       manifest->filelist != NULL && manifest->filelist->file_version == 1);
}

void test_history() {}

void test_rollback() {}




void test_update() {}

void test_upgrade() {}

void test_destroy() {}

int main(int argc, char** argv) {
  test_configure();
  test_create();
  test_add();
  test_remove();
  test_checkout();
  test_currentversion();
  test_commit();

  return 0;
}
