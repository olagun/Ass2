#include "src/configure.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/read.h"

Configure* configure_new() { return calloc(1, sizeof(Configure)); }

void configure_write(char* ip, char* port) {
  int fd = creat(".configure", 0777);
  dprintf(fd, "%s:%s\n", ip, port);
  close(fd);
}

// Returns NULL if unable to read
Configure* configure_read() {
  int fd = open(".configure", O_RDONLY, 0777);
  if (fd < 0) return NULL;

  Configure* config = configure_new();
  config->ip = read_until(fd, ':');
  config->port = read_until(fd, '\n');
  close(fd);

  return config;
}