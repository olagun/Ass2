
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/util/color.h"
#include "src/util/get_file_size.h"

// ANSWERS are in "answers" and are organized by command name

// to run tests...

// 1. make build_test - moves server and client and "answers" into "test", and
// compiles "wtf_test"

// RUN SERVER FIRST

// 2. make run_test - does cd test then runs WTFTest

void test_configure() {
  system("cd client;");
  // I really wonder how this test will work on iLab considering
  // that most ports will be taken
  system("cd client; ./WTF configure 127.0.0.1 8000");

  // `diff` finds the difference between two files
  // idk what `-q` means but you need it
  // `>` outputs the difference bewteen the answer and what configure generates
  // to "diff.txt". if diff.txt is empty, configure runs correctly
  system(
      "cd client; diff -q .configure ../answers/configure/.configure > "
      "../diff.txt");

  if (get_file_size("diff.txt") == 0) {
    printf(GRN "configure passes tests\n" RESET);
  } else {
    printf(RED "configure doesn't pass tests\n" RESET);
  }

  // remove diff.txt
  system("rm -f diff.txt");
}

void test_create() {}

void test_add() {}

void test_remove() {}

void test_checkout() {}

void test_currentversion() {}

void test_history() {}

void test_rollback() {}

void test_commit() {}

void test_push() {}

void test_update() {}

void test_upgrade() {}

void test_destroy() {}


int main(int argc, char** argv) {
  test_configure();

  return 0;
}
