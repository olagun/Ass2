
#include "src/accept.h"

#include <string.h>

#include "src/commands/create.h"
#include "src/commands/currentversion.h"
#include "src/request.h"
#include "src/response.h"

// Accepts and responds to client requests
// This is where most things happen
Response* accept(Request* request) {
  char* command_name = request->command_name;

  if (strcmp("create", command_name) == 0) {
    return create_server(request);
  }

  if (strcmp("currentversion", command_name) == 0) {
    return currentversion_server(request);
  }

  // if (strcmp("<command_name>", command) == 0) {
  //   return <Command_name>_server(body);
  // }

  return NULL;
}
