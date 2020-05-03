#include <stdio.h>
#include <string.h>

#include "src/client.h"
#include "src/manifest.h"
#include "src/request.h"
#include "src/response.h"
#include "src/util/directory_exists.h"

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
    printf("file_version: %d\n", item->file_version);

    item = item->next;
  }
}

Response* currentversion_server(Request* request) {
  char s[1000] = {0};
  sprintf(s, "projects/%s", request->project_name);

  if (!directory_exists(s)) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = "[Current Version Error] Project does not exist";
    return response;
  }

  // Read in manifest from project
  Manifest* manifest = manifest_read(s);
  if (manifest == NULL){
    Response* response = response_new();
    response->status_code = -1;
    response->message = "[Current Version Error] Could not read .Manifest";
    return response;
  }

  // Send manifest files
  Response* response = response_new();
  response->filelist = manifest->filelist;

  return response;
}
