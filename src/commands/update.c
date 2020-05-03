#include "src/commands/update.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/client.h"
#include "src/commands/commit.h"
#include "src/filelist.h"
#include "src/util/color.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"
#include "src/util/get_server_manifest.h"

void update_client(char *project_name) {
  // Fail if local version doesn't exist
  if (!directory_exists(project_name)) {
    printf(BRED "[Update Error]" RESET
                " Project '%s' does not exist locally. Run `checkout` or "
                "`create` first\n",
           project_name);
    return;
  }

  // Client .Manifest
  Manifest *client_manifest = manifest_read(project_name);
  if (client_manifest == NULL) {
    printf(BRED "[Update Error]" RESET
                " Could not read client manifest for project '%s'\n",
           project_name);
    return;
  }

  // Server .Manifest
  Manifest *server_manifest = get_server_manifest(project_name);
  if (server_manifest == NULL) {
    printf(BRED "[Update Error]" RESET
                " Could not read server manifest for project '%s'\n",
           project_name);
    return;
  }

  char update_path[1000] = {0};
  char conflict_path[1000] = {0};

  sprintf(update_path, "%s/.Update", project_name);
  sprintf(conflict_path, "%s/.Conflict", project_name);

  int update_fd = creat(update_path, 0777);
  int conflict_fd = creat(conflict_path, 0777);

  // Update code
  if (server_manifest->project_version == client_manifest->project_version) {
    close(update_fd);
    close(conflict_fd);

    remove(conflict_path);  // Delete .Conflict file

    printf(BWHT "[Update]" RESET " Up To Date\n");
    return;
  }

  FileList *client_files = client_manifest->filelist;
  FileList *server_files = server_manifest->filelist;

  int conflict_count = 0;
  FileList *server_file = server_files;

  while (server_file != NULL) {
    FileList *client_file = get_file_from(client_files, server_file->file_path);

    // File path
    char *server_file_path = server_file->file_path;
    char *client_file_path = client_file == NULL ? "" : client_file->file_path;

    // File version
    int server_file_version = server_file->file_version;
    int client_file_version =
        client_file == NULL ? -1 : client_file->file_version;

    // Hashes
    char *client_hash = client_file == NULL ? "" : client_file->file_hash;
    char *server_hash = server_file->file_hash;
    char *live_hash = client_file == NULL
                          ? ""
                          : get_file_hash(project_name, client_file_path);

    // Criteria
    bool client_live_hash_matches =
        client_file == NULL ? false : strcmp(client_hash, live_hash) == 0;
    bool client_server_hash_matches =
        client_file == NULL ? false : strcmp(server_hash, client_hash) == 0;
    bool client_server_version_matches =
        client_file == NULL ? false
                            : client_file_version == server_file_version;

    // Modify code
    if (!client_server_version_matches && !client_server_hash_matches &&
        client_live_hash_matches) {
      dprintf(update_fd, "M %s %s\n", server_file->file_path,
              server_file->file_hash);
      printf("M %s\n", server_file->file_path);
    }

    // Add code
    if (client_file == NULL) {
      dprintf(update_fd, "A %s %s\n", server_file->file_path,
              server_file->file_hash);
      printf("A %s\n", server_file->file_path);
    }

    // Conflict code
    if (client_file != NULL && !client_server_hash_matches &&
        !client_live_hash_matches) {
      dprintf(conflict_fd, "C %s %s\n", server_file->file_path,
              server_file->file_hash);
      printf("C %s\n", server_file->file_path);

      conflict_count++;  // Increment conflicts
    }

    server_file = server_file->next;
  }

  if (conflict_count > 0) {
    printf(BWHT "[Update]" RESET
                " %d conflicts found, resolve them before updating\n",
           conflict_count);
  }

  // Delete code
  FileList *client_file = client_files;
  while (client_file != NULL) {
    FileList *server_file = get_file_from(server_files, client_file->file_path);

    if (server_file == NULL) {
      dprintf(update_fd, "D %s %s\n", client_file->file_path,
              client_file->file_hash);
      printf("D %s\n", client_file->file_path);
    }

    client_file = client_file->next;
  }

  close(update_fd);
  close(conflict_fd);

  if (conflict_count > 0) {
    remove(update_path);
  } else {
    remove(conflict_path);
  }
}
