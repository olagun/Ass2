#include "src/read.h"

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/token.h"

// Reads from a file descriptor unitl it finds `\n`
char* read_line(int fd) { return read_until(fd, '\n'); }

// Reads from a file descriptor unitl it finds `delim`
char* read_until(int fd, char delim) {
  char* tmp = calloc(2, sizeof(char));
  Token* head = NULL;

  while (read(fd, tmp, 1) > 0) {
    if (tmp[0] == delim) break;
    head = token_append(head, token_new(tmp));
  }

  return token_string(head);
}

// TODO: Check if unsuccessful
// Reads `n` bytes from a file descriptor
char* read_nbytes(int fd, int n) {
  char* tmp = calloc(n + 1, sizeof(char));
  read(fd, tmp, n);
  return tmp;
}