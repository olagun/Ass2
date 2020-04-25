#include "src/filelist.h"

#ifndef REQUEST_H
#define REQUEST_H

// Client request
typedef struct Request {
  char* message;
  int status_code;

  char* command_name;
  char* project_name;

  // Manifest
  int project_version;
  int file_count;
  FileList* filelist;
} Request;

Request* request_new();
Request* request_read(int fd);
void request_write(int fd, Request* request);
void request_log(Request* request);

#endif