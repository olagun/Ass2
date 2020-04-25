#include <stdio.h>
#include <string.h>

#include "src/client.h"
#include "src/manifest.h"
#include "src/request.h"
#include "src/response.h"

void currentversion_client(char* project_name) {
  // Create request
  Request* request = request_new();
  request->command_name = "currentversion";
  request->project_name = project_name;

  // Send request
  Response* response = client_send(request);

  // Print files in response
  FileList* item = response->filelist;
  while (item != NULL) {
    printf("file_path: %s/%s\n", project_name, item->file_path);
    printf("file_version: %s/%d\n", project_name, item->file_version);

    item = item->next;
  }
}

Response* currentversion_server(Request* request) {
  // Read in manifest from project
  Manifest* manifest = manifest_read(request->project_name);

  // Send manifest files
  Response* response = response_new();
  response->filelist = manifest->filelist;

  // Log?

  return response;
}
