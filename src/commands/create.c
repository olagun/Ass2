#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/client.h"
#include "src/manifest.h"
#include "src/request.h"
#include "src/response.h"
#include "src/util/color.h"

// Client
void create_client(char* project_name) {
  // Create request
  Request* request = request_new();
  request->command_name = "create";
  request->project_name = project_name;

  // Send request
  Response* response = client_send(request);
  if (response == NULL) {
    printf("No response recieved\n");
  } else {
    FileList* filelist = response->filelist;
    filelist_write(project_name, response->filelist);
  }
}

// Server
Response* create_server(Request* request) {
  int project_name_length = strlen(request->project_name);
  char* project_path = calloc(project_name_length + 50, sizeof(char));
  char* manifest_path = calloc(project_name_length + 50, sizeof(char));

  // Create paths for `.Manifest` and project
  sprintf(project_path, "projects/%s", request->project_name);
  sprintf(manifest_path, "projects/%s/.Manifest", request->project_name);

  // Create project folder
  if (mkdir(project_path, 0777) < 0) return NULL;

  // Create manifest file and write version number
  if (creat(manifest_path, 0777) < 0) return NULL;
  int manifest_fd = creat(manifest_path, 0777);
  dprintf(manifest_fd, "0\n");  // Write version number
  close(manifest_fd);

  // Get files in manifest (only .Manifest should exist)
  Manifest* manifest = manifest_read(project_path);

  // Write response
  Response* response = response_new();
  response->message = "Success!";
  response->filelist = filelist_readbytes(project_path, manifest->filelist);

  // Log
  printf("\n");
  printf(BWHT "create " BLU "%s" RESET "\n", request->project_name);

  return response;
}
