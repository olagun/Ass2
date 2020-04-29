#include "src/response.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/read.h"
#include "src/util/color.h"

Response* response_new() { return calloc(1, sizeof(Response)); }

// Writes response according to protocol
void response_write(int fd, Response* response) {
  if (response == NULL) {
    response_log(response);
    return;
  }

  // Add file count
  response->file_count = filelist_size(response->filelist);

  // Write response
  dprintf(fd, "%s:", response->message);          // <message>:
  dprintf(fd, "%d:", response->status_code);      // <status_code>:
  dprintf(fd, "%d:", response->project_version);  // <project_version>:
  dprintf(fd, "%d:", response->file_count);       // <file_count>:

  FileList* item = response->filelist;
  while (item != NULL) {                           // Write every file
    dprintf(fd, "%s:", item->file_path);           // <file_path>:
    dprintf(fd, "%d:", item->file_version);        // <file_version>:
    dprintf(fd, "%s:", item->file_hash);           // <file_hash>:
    dprintf(fd, "%d:", item->file_size);           // <file_size>:
    write(fd, item->file_bytes, item->file_size);  // <file_bytes>

    item = item->next;
  }

  response_log(response);
}

// Reads response according to protocol
Response* response_read(int fd) {
  if (fd < 0) {
    response_log(NULL);
    return NULL;
  }

  // Read response
  Response* response = response_new();
  response->message = read_until(fd, ':');                // <message>:
  response->status_code = atoi(read_until(fd, ':'));      // <status_code>:
  response->project_version = atoi(read_until(fd, ':'));  // <project_version>:
  response->file_count = atoi(read_until(fd, ':'));       // <file_count>:

  int n = response->file_count;
  for (; n > 0; n--) {  // Read in every file
    FileList* item = filelist_new();
    item->file_path = read_until(fd, ':');                // <file_path>:
    item->file_version = atoi(read_until(fd, ':'));       // <file_version>:
    item->file_hash = read_until(fd, ':');                // <file_hash>:
    item->file_size = atoi(read_until(fd, ':'));          // <file_size>:
    item->file_bytes = read_nbytes(fd, item->file_size);  // <file_bytes>

    // Append item to list
    response->filelist = filelist_append(response->filelist, item);
  }

  response_log(response);

  return response;
}

void response_log(Response* response) {
  printf("\n");
  printf("╭" BWHT " Response\n" RESET);

  if (response != NULL) {
    printf("├ message\t" BLU "%s" RESET "\n", response->message);
    printf("├ status_code\t" BLU "%d" RESET "\n", response->status_code);

    printf("├ version\t" BLU "%d" RESET "\n", response->project_version);
    printf("├ file_count\t" BLU "%d" RESET "\n", response->file_count);
    printf("├ files\t\n");
    FileList* item = response->filelist;
    while (item != NULL) {
      printf("├── " BLU "%s" RESET "\n", item->file_path);
      printf("├── file size:\t" BLU "%d" RESET "\n", item->file_size);

      if (item->file_size < 100) {
        printf("├── file bytes:\t" YEL "$" BLU "%.*s" YEL "$" RESET "\n",
               item->file_size, item->file_bytes);
      } else {
        printf("├── file bytes:\tfile too large too print\n");
      }

      item = item->next;
    }
  }

  printf("╰─\n");
}