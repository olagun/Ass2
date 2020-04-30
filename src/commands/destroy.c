// The destroy command will fail if the project name doesn’t exist on the server
// or the client can not communicate with it. On receiving a destroy command the
// server should lock the repository, expire any pending commits, delete all
// files
// and subdirectories under the project and send back a success message.

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "src/client.h"
#include "src/util/directory_exists.h"

void destroy_client(char* project_name) {
  Request* request = request_new();
  request->command_name = "destroy";
  request->project_name = project_name;

  Response* response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

  printf("Destroyed project %s", project_name);
}

Response* destroy_server(Request* request) {
  char* project_name = request->project_name;

  char* project_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(project_path, "projects/%s", project_name);

  if (!directory_exists(project_path)) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = "Project doesn't exist.";
    return response;
  }

  char* sys_commit = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(sys_commit, "rm -rf commits/%s", project_name);

  char* sys_history = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(sys_history, "rm -rf history/%s", project_name);

  char* sys_project = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(sys_project, "rm -rf projects/%s", project_name);

  // TODO(Sam): Is removing with system commands safe? Is there a better way to
  // do this?
  system(sys_commit);
  system(sys_history);
  system(sys_project);

  Response* response = response_new();
  response->status_code = 1;
  response->message = "Success";
  return response;
}