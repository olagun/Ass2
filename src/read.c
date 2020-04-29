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

// Reads `n` bytes from a file descriptor
char* read_nbytes(int fd, int n) {
  char* tmp = calloc(n + 1, sizeof(char));
  int bytes_read = 0;
  int total_read = 0;

  while (total_read < n && (bytes_read = read(fd, tmp + total_read, n)) > 0) {
    total_read = total_read + bytes_read;
  }

  int bytes_missing = n - total_read;
  if (bytes_missing > 0) {
    printf("%d bytes were not read from fd %d\n", n - total_read, fd);
  }

  return tmp;
}