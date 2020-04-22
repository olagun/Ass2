#include <stdio.h>
#include <string.h>

#include "../../src/client/client.h"
#include "../../src/manifest/manifest.h"
#include "../../src/request/request.h"
#include "../../src/response/response.h"

void currentversion_client(char* project_name) {
  Request* request = request_new();
  request->command_name = "currentversion";

  Response* response = client_send(request);

  FileList* item = response->filelist;
  while (item != NULL) {
    if (strcmp(".Manifest", item->file_path) != 0) {
      printf("file_path: %s/%s\n", project_name, item->file_path);
      printf("file_version: %s/%d\n", project_name, item->file_version);
    }

    item = item->next;
  }
}

Response* currentversion_server(Request* request) {
  Manifest* manifest = manifest_read(request->project_name);

  Response* response = response_new();
  response->filelist = manifest->filelist;

  return response;
}
