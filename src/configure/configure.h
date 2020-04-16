#ifndef CONFIGURE_H
#define CONFIGURE_H

typedef struct {
  char* ip;
  char* port;
} Configure;

void Configure_write(char* ip, char* port);
Configure* Configure_read();

#endif