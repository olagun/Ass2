#include "src/commands/commit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/client.h"
#include "src/util/directory_exists.h"
#include "src/util/get_file_size.h"

void commit_client(char* project_name) {
  Request* request = request_new();
  request->command_name = "commit";
  request->project_name = project_name;

  Response* response = client_send(request);

  if (response->status_code < 0) {
    printf("%s\n", response->message);
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