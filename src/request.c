#include "src/request.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/read.h"
#include "src/util/color.h"

Request* request_new() { return calloc(1, sizeof(Request)); }

// Writes request according to protocol
void request_write(int fd, Request* request) {
  if (request == NULL) return;

  request->file_count = filelist_size(request->filelist);

  dprintf(fd, "%s:", request->command_name);     // <command_name>:
  dprintf(fd, "%s:", request->project_name);     // <project_name>:
  dprintf(fd, "%d:", request->project_version);  // <project_version>:
  dprintf(fd, "%d:", request->file_count);       // <file_count>:

  FileList* item = request->filelist;
  while (item != NULL) {
    dprintf(fd, "%s:", item->file_path);           // <file_path>:
    dprintf(fd, "%d:", item->file_size);           // <file_size>:
    write(fd, item->file_bytes, item->file_size);  // <file_bytes>

    item = item->next;
  }

  request_log(request);
}

// Reads request according to protocol
Request* request_read(int fd) {
  if (fd < 0) return NULL;

  // Read response
  Request* request = request_new();
  request->command_name = read_until(fd, ':');           // <command_name>:
  request->project_name = read_until(fd, ':');           // <project_name>:
  request->project_version = atoi(read_until(fd, ':'));  // <project_version>:
  request->file_count = atoi(read_until(fd, ':'));       // <file_count>:

  int n = request->file_count;
  for (; n > 0; n--) {  // Read in every file
    FileList* item = filelist_new();
    item->file_path = read_until(fd, ':');                // <file_path>:
    item->file_size = atoi(read_until(fd, ':'));          // <file_size>:
    item->file_bytes = read_nbytes(fd, item->file_size);  // <file_bytes>

    // Append item to list
    request->filelist = filelist_append(request->filelist, item);
  }

  request_log(request);

  return request;
}

void request_log(Request* request) {
  printf("\n");
  printf("╭" BWHT " Request" RESET "\n");
  printf("├ command_name\t" BLU "%s" RESET "\n", request->command_name);
  printf("├ project_name\t" BLU "%s" RESET "\n", request->project_name);
  printf("├ version\t" BLU "%d" RESET "\n", request->project_version);
  printf("├ file_count\t" BLU "%d" RESET "\n", request->file_count);
  printf("├ files\t\n");

  FileList* item = request->filelist;
  while (item != NULL) {
    printf("├── " BLU "%s" RESET "\n", item->file_path);
    printf("├── file size:\t" BLU "%d" RESET "\n", item->file_size);
    printf("├── file bytes:\t" YEL "$" BLU "%.*s" YEL "$" RESET "\n",
           item->file_size, item->file_bytes);

    item = item->next;
  }

  printf("╰─\n");
}