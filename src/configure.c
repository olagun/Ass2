#include "src/configure.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/read.h"
#include "src/token.h"

Configure* configure_new() { return calloc(1, sizeof(Configure)); }

void configure_write(char* ip, char* port) {
  int configure_fd = creat(".configure", 0777);
  dprintf(configure_fd, "%s:%s\n", ip, port);
  close(configure_fd);
}

// TODO: Check if file is empty
Configure* configure_read() {
  int configure_fd = open(".configure", O_RDONLY, 0777);

  Configure* config = configure_new();
  config->ip = read_until(configure_fd, ':');
  config->port = read_until(configure_fd, '\n');

  close(configure_fd);

  return config;
}