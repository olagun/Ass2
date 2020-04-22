#include "response.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../color/color.h"
#include "../filelist/filelist.h"
#include "../read/read.h"

Response* response_new() { return calloc(1, sizeof(Response)); }

void response_write(int fd, Response* response) {
  if (response == NULL) return;

  response->file_count = filelist_size(response->filelist);

  dprintf(fd, "%s:", response->message);     //  <message>:
  dprintf(fd, "%d:", response->file_count);  //  <file_count>:

  FileList* item = response->filelist;
  while (item != NULL) {
    dprintf(fd, "%s:", item->file_path);           // <file_path>:
    dprintf(fd, "%d:", item->file_size);           // <file_size>:
    write(fd, item->file_bytes, item->file_size);  // <file_bytes>

    item = item->next;
  }

  response_log(response);
}

Response* response_read(int fd) {
  Response* response = response_new();
  response->message = read_until(fd, ':');           //  <message>:
  response->file_count = atoi(read_until(fd, ':'));  //  <file_count>:

  int n = response->file_count;
  for (; n > 0; n--) {
    FileList* item = filelist_new();

    item->file_path = read_until(fd, ':');                // <file_path>:
    item->file_size = atoi(read_until(fd, ':'));          // <file_size>:
    item->file_bytes = read_nbytes(fd, item->file_size);  // <file_bytes>

    // prepend item
    item->next = response->filelist;
    response->filelist = item;
  }

  response_log(response);

  return response;
}

// Just logs responses
void response_log(Response* response) {
  printf("\n");
  printf(YEL "$" RESET
             " = file start and file end)"
             "\n");

  printf("╭" BWHT " Response\n" RESET);
  printf("├ message\t" BLU "%s" RESET "\n", response->message);
  printf("├ file_count\t" BLU "%d" RESET "\n", response->file_count);
  printf("├ files\t\n");

  FileList* item = response->filelist;
  while (item != NULL) {
    printf("├── " BLU "%s" RESET "\n", item->file_path);
    printf("├── file size:\t" BLU "%d" RESET "\n", item->file_size);
    printf("├── file bytes:\t" YEL "$" BLU "%.*s" YEL "$" RESET "\n",
           item->file_size, item->file_bytes);

    item = item->next;
  }

  printf("╰─\n");
}