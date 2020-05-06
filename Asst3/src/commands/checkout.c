#include "src/commands/checkout.h"

#include <dirent.h>
#include <sys/stat.h>

#include "src/manifest.h"
#include "src/util/color.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_server_manifest.h"

void checkout_client(char* project_name) {
  // Fail if the project name already exists on the client side
  if (directory_exists(project_name)) {
    printf(BRED "[Checkout Error]" RESET
                " Project name already exists on the client side\n");
    return;
  }

  // Fail if configure was not run on the client side
  if (!file_exists(".configure")) {
    printf(BRED "[Checkout Error]" RESET " Run configure before continuing\n");
    return;
  }

  // Fail if the project doesn't exist on the server
  Manifest* server_manifest = get_server_manifest(project_name);
  if (server_manifest == NULL) {
    printf(BRED "[Checkout Error]" RESET
                " Project '%s' doesn't exist on the server\n",
           project_name);
    return;
  }

  // Create project directory
  if (mkdir(project_name, 0777) != 0) {
    printf(BRED "[Checkout Error]" RESET
                " Unable to create the project directory '%s'\n",
           project_name);
  }

  // Save files and manifest to client
  filelist_write(project_name, server_manifest->filelist);
  manifest_write(project_name, server_manifest);
}
