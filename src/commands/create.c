#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/client.h"
#include "src/manifest.h"
#include "src/mutexlist.h"
#include "src/request.h"
#include "src/response.h"
#include "src/testing.h"
#include "src/util/color.h"

// Client
void create_client(char* project_name) {
  // Create request
  Request* request = request_new();
  request->command_name = "create";
  request->project_name = project_name;

  // Send request
  Response* response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
  } else {
    FileList* filelist = response->filelist;

    mkdir(project_name, 0777);

    // Write files
    filelist_write(project_name, response->filelist);

    // Write manifest
    Manifest* manifest = manifest_new();
    manifest->filelist = response->filelist;
    manifest->project_version = response->project_version;
    manifest_write(project_name, manifest);
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
  if (mkdir(project_path, 0777) < 0) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = errno == EEXIST ? "Project folder already exists"
                                        : "Couldn't create a project folder";
    return response;
  }

  // Add mutex for project folder directly after creating it
  // [Add] Project
  add_project_mutex(request->project_name);

  // [Lock] Project
  lock_project(request->project_name);

  // Create manifest file and write version number
  if (creat(manifest_path, 0777) < 0) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = "Couldn't create a manifest folder";

    // [Unlock] Project
    unlock_project(request->project_name);
    return response;
  }

  int manifest_fd = creat(manifest_path, 0777);
  dprintf(manifest_fd, "0\n");  // Write version number
  close(manifest_fd);

  // Write response
  Response* response = response_new();
  response->message = "Success!";

  // Get files in manifest (only .Manifest should exist)
  Manifest* manifest = manifest_read(project_path);
  response->project_version = manifest->project_version;
  response->filelist = filelist_readbytes(project_path, manifest->filelist);

  // Log
  if (!TESTING) {
    printf("\n");
    printf("Created new project " BLU "%s" RESET "\n", request->project_name);
    printf("\n");
  }

  // sleep(5);

  // [Unlock] Project
  unlock_project(request->project_name);
  return response;
}
