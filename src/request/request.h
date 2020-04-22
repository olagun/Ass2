#include "../filelist/filelist.h"

#ifndef REQUEST_H
#define REQUEST_H

// Client request
typedef struct Request {
  char* command_name;   // Command
  char* project_name;   // Project
  int project_version;  // Project version (Used for rollback)

  // Files
  int file_count;
  FileList* filelist;
} Request;

Request* request_new();
Request* request_read(int fd);
void request_write(int fd, Request* request);
void request_log(Request* request);

#endif