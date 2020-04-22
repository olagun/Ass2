#include "../filelist/filelist.h"

#ifndef RESPONSE_H
#define RESPONSE_H

// Server response
typedef struct Response {
  // Message
  char* message;

  // Files
  int file_count;
  FileList* filelist;
} Response;

Response* response_new();
Response* response_read(int fd);
void response_write(int fd, Response* response);
void response_log(Response* response);

#endif