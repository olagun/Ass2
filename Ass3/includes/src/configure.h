#ifndef CONFIGURE_H
#define CONFIGURE_H

typedef struct Configure {
  char* ip;
  char* port;
} Configure;

Configure* configure_new();
void configure_write(char* ip, char* port);
Configure* configure_read();

#endif