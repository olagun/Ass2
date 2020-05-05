#include "src/filelist.h"
#include "src/manifest.h"

#ifndef RESPONSE_H
#define RESPONSE_H

// Server response
typedef struct Response {
  char* message;
  int status_code;

  // Manifest
  int project_version;
  int file_count;
  FileList* filelist;
} Response;

Response* response_new();
Response* response_read(int fd);
void response_write(int fd, Response* response);
void response_log(Response* response);

#endif