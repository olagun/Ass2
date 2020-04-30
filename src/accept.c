
#include "src/accept.h"

#include <string.h>

#include "src/commands/commit.h"
#include "src/commands/create.h"
#include "src/commands/currentversion.h"
#include "src/commands/destroy.h"
#include "src/commands/history.h"
#include "src/commands/push.h"
#include "src/commands/rollback.h"
#include "src/commands/update.h"
#include "src/manifest.h"
#include "src/request.h"
#include "src/response.h"

// Accepts and responds to client requests
// This is where most things happen
Response* on_accept(Request* request) {
  char* command_name = request->command_name;

  if (strcmp("get_server_manifest", command_name) == 0) {
    char* project_path =
        calloc(strlen(request->project_name) + 50, sizeof(char));
    sprintf(project_path, "projects/%s", request->project_name);

    Manifest* manifest = manifest_read(project_path);

    if (manifest == NULL) {
      Response* response = response_new();
      response->status_code = -1;
      return response;
    }

    // Read `get_server_manifest` for why it's done this way
    Response* response = response_new();
    response->project_version = manifest->project_version;
    response->filelist = manifest->filelist;
    return response;
  }

  if (strcmp("destroy", command_name) == 0) {
    return destroy_server(request);
  }

  if (strcmp("history", command_name) == 0) {
    return history_server(request);
  }

  if (strcmp("create", command_name) == 0) {
    return create_server(request);
  }

  if (strcmp("currentversion", command_name) == 0) {
    return currentversion_server(request);
  }

  if (strcmp("commit", command_name) == 0) {
    return commit_server(request);
  }

  if (strcmp("push", command_name) == 0) {
    return push_server(request);
  }

  if (strcmp("update", command_name) == 0) {
    return update_server(request);
  }

  if (strcmp("upgrade", command_name) == 0) {
    return update_server(request);
  }

  if (strcmp("rollback", command_name) == 0) {
    return rollback_server(request);
  }

  // if (strcmp("<command_name>", command) == 0) {
  //   return <Command_name>_server(body);
  // }

  return NULL;
}
