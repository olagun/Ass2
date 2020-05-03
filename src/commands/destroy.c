#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "src/client.h"
#include "src/mutexlist.h"
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

  printf(BWHT "[Destroy]" RESET " Successfully destroyed '%s'\n", project_name);
}

Response* destroy_server(Request* request) {
  char* project_name = request->project_name;

  char* project_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(project_path, "projects/%s", project_name);

  if (!directory_exists(project_path)) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = "[Destroy Error] Project doesn't exist.";
    return response;
  }

  // [Lock] Project
  lock_project(request->project_name);

  char s[1000] = {0};

  sprintf(s, "rm -rf commits/%s", project_name);
  system(s);

  sprintf(s, "rm -rf history/%s", project_name);
  system(s);

  sprintf(s, "rm -rf projects/%s", project_name);
  system(s);

  // [Test Destroy Mutex]
  // sleep(5);

  // [Unlock] Project
  unlock_project(request->project_name);

  // [Remove] Project
  remove_project_mutex(request->project_name);

  Response* response = response_new();
  response->message = "[Destroy] Successfully destroyed project";
  return response;
}