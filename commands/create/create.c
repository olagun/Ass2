// Fails for 2 reasons - if the project name already exists on  the server or
// the client can not communicate with the server. Otherwise,
// the server will create a project folder with the given name, initialize a
//.Manifest for it and send it to the client. The client will set up a local
// version of the project folder in its current directory and should place the
//.Manifest the server sent in it.

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../src/client/client.h"
#include "../../src/color/color.h"

void Create_client(char* project_name) {
  char message[1000];
  sprintf(message, "create:%s:", project_name);
  char* response = Client_send(message);
}

char* Create_server(char* body) {
  // there's gotta be a better way to append strings...
  char* project_name = strtok(strdup(body), ":");

  char* project_path = malloc(strlen("projects/") + strlen(project_name) + 1);
  strcpy(project_path, "projects/");
  strcat(project_path, project_name);

  char* manifest_path = malloc(strlen("projects/") + strlen(project_name) +
                               strlen(".Manifest") + 1);
  strcpy(manifest_path, "projects/");
  strcat(manifest_path, project_name);
  strcat(manifest_path, "/.Manifest");

  // Create project folder
  if (mkdir(project_path, 0777) < 0) {
    return "failure:";
  }

  // Create manifest file
  if (creat(manifest_path, 0777) < 0) {
    return "failure:";
  }
  // Write version number to manifest
  Token_write(manifest_path, Token_new("0"));

  printf(GRN "Success!" RESET " created project '" BLU "%s" RESET "'\n",
         project_name);
 
  // should return serialized project. "created project" is a placeholder
  // return serialize(project_name)
  return "created project";
}
