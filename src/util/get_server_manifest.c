#include <stdio.h>

#include "src/client.h"
#include "src/manifest.h"
#include "src/request.h"
#include "src/response.h"

// Client function to request manifest from server.
// Check `accept.c` for more info on what the server does.
Manifest* get_server_manifest(char* project_name) {
  Request* request = request_new();
  request->command_name = "get_server_manifest";
  request->project_name = project_name;

  Response* response = client_send(request);
  if (response->status_code < 0) {
    return NULL;
  }

  Manifest* manifest = manifest_new();
  manifest->project_version = response->project_version;
  manifest->filelist = response->filelist;

  return manifest;
}