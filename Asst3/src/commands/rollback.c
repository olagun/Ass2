
#include "src/commands/rollback.h"

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
#include "src/compression.h"
#include "src/util/directory_exists.h"

void rollback_client(char* project_name, char* project_version) {
  Request* request = request_new();
  request->command_name = "rollback";
  request->project_name = project_name;
  request->project_version = atoi(project_version);

  Response* response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

  printf("Successful rollback to version %s", project_version);
}

Response* rollback_server(Request* request) {
  // Project name
  char* project_name = request->project_name;

  // Project path
  char* project_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(project_path, "projects/%s", project_name);

  if (!directory_exists(project_path)) {
    Response* response = response_new();
    response->message = "[Rollback Error] Project doesn't exist";
    response->status_code = -1;
    return response;
  }

  // Rollback version
  int rollback_version = request->project_version;

  // Fail if rollback version is the current version
  Manifest* manifest = manifest_read(project_path);
  if (manifest->project_version == rollback_version) {
    Response* response = response_new();
    response->message = "[Rollback Error] Project is already at this version";
    response->status_code = -1;
    return response;
  }

  /*
    Path to the desired project version directory inside of `history/`

    For reference, `history/` is organized as:
    history/<project_name>/<project_version>/
  */
  char* version_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(version_path, "history/%s/%d", project_name, rollback_version);

  // Fail if the rollback project version doesn't exist
  if (!directory_exists(version_path)) {
    Response* response = response_new();
    response->message =
        "[Rollback Error] Could not find project version on the server.";
    response->status_code = -1;
    return response;
  }

  // Open `history/`
  char* history_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(history_path, "history/%s", project_name);
  DIR* dir = opendir(history_path);
  if (dir == NULL) {
    perror("[Rollback Error]");
    Response* response = response_new();
    response->message = "[Rollback Error] Could not find project history.";
    response->status_code = -1;
    return response;
  }

  // Iterate through all project versions
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    int version = atoi(entry->d_name);

    // Delete project versions more current than the rollback version
    if (version > rollback_version) {
      char* sys_remove = calloc(strlen(project_name) + 50, sizeof(char));
      sprintf(sys_remove, "rm -rf history/%s/%d", project_name, version);
      system(sys_remove);
    }
  }

  closedir(dir);

  // Delete the current project
  char* sys_rm = calloc(strlen(project_name) * 2 + 50, sizeof(char));
  sprintf(sys_rm, "rm -rf projects/%s", project_name);
  system(sys_rm);

  // Decompress, write, and delete `.History`
  char history_archive[1000] = {0};
  sprintf(history_archive, "history/%s/%d/.History", project_name,
          rollback_version);

  int open_fd = open(history_archive, O_RDONLY, 0777);

  FileList* filelist = read_compressed_filelist(open_fd);
  mkdir(project_path, 0777);
  filelist_write(project_path, filelist);

  close(open_fd);
  remove(history_archive);

  // Move `.Manifest`
  char manifest_sys[1000] = {0};
  sprintf(manifest_sys, "mv history/%s/%d/.Manifest projects/%s", project_name,
          rollback_version, project_name);
  system(manifest_sys);

  // Remove folder
  char sys_rm_h[1000] = {0};
  sprintf(sys_rm_h, "rm -rf history/%s/%d", project_name, rollback_version);
  system(sys_rm_h);

  Response* response = response_new();
  response->status_code = 1;
  response->message = "Success!";
  return response;
}