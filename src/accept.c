
#include "src/accept.h"

#include <string.h>

#include "src/commands/commit.h"
#include "src/commands/create.h"
#include "src/commands/currentversion.h"
#include "src/commands/push.h"
#include "src/commands/update.h"
#include "src/request.h"
#include "src/response.h"

// Accepts and responds to client requests
// This is where most things happen
Response* on_accept(Request* request) {
  char* command_name = request->command_name;

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

  // if (strcmp("<command_name>", command) == 0) {
  //   return <Command_name>_server(body);
  // }

  return NULL;
}
