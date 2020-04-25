#include "src/commands/commit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/client.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_size.h"

void commit_client(char* project_name) {
  char* update_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(update_path, "projects/%s/.Update", project_name);

  char* conflict_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(conflict_path, "projects/%s/.Conflict", project_name);

  if ((file_exists(update_path) && get_file_size(update_path) > 0) ||
      file_exists(conflict_path)) {
    printf("Contains .Update or .Conflict file. Could not be commit\n");
    return;
  }

  Request* request = request_new();
  request->command_name = "commit";
  request->project_name = project_name;

  Response* response = client_send(request);

  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

  Manifest* local_manifest = manifest_read(project_name);

  // Exit if the versions don't match
  if (response->project_version != local_manifest->project_version) {
    printf("Manifest's don't match. Update your local files first.\n");
    return;
  }
}

Response* commit_server(Request* request) {
  char* full_path = calloc(strlen(request->project_name) + 50, sizeof(char));
  sprintf(full_path, "projects/%s", request->project_name);

  if (!directory_exists(full_path)) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = "Project doesn't exist on the server";
    return response;
  }

  return NULL;
}