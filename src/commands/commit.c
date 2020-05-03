#include "src/commands/commit.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
#include "src/commands/commands.h"
#include "src/filelist.h"
#include "src/testing.h"
#include "src/util/color.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"
#include "src/util/get_server_manifest.h"

void commit_client(char *project_name) {
  char commit_path[1000] = {0};
  sprintf(commit_path, "%s/.Commit", project_name);

  char update_path[1000] = {0};
  sprintf(update_path, "%s/.Update", project_name);

  char conflict_path[1000] = {0};
  sprintf(conflict_path, "%s/.Conflict", project_name);

  // Fail if has non-empty `.Update` file
  if (file_exists(update_path) && get_file_size(update_path) > 0) {
    printf(BRED "[Commit Error]" RESET
                " Contains non-empty `.Update` file. `upgrade` before "
                "commiting.\n");
    return;
  }

  // Fail if has `.Conflict` file
  if (file_exists(conflict_path)) {
    printf(BRED "[Commit Error]" RESET
                "Contains `.Conflict` file. Resolve conflict and "
                "`upgrade` before commiting.\n");
    return;
  }

  // 1) Request manifest from server
  {
    Manifest *server_manifest = get_server_manifest(project_name);
    Manifest *local_manifest = manifest_read(project_name);

    // Check if manifest versions match
    if (server_manifest->project_version != local_manifest->project_version) {
      printf(BRED "[Commit Error]" RESET " Update your local files first.\n");
      return;
    }

    // Create commit file
    int commit_fd = creat(commit_path, 0777);
    if (commit_fd < 0) {
      printf(BRED "[Commit Error]" RESET " Could not create `.Commit`.\n");
      return;
    }

    FileList *local_files = local_manifest->filelist;
    FileList *server_files = server_manifest->filelist;

    FileList *curr_local = local_files;
    FileList *curr_server = NULL;

    while (curr_local != NULL) {
      char *file_path = curr_local->file_path;

      // Will be NULL if doesn't exist on the server
      curr_server = get_file_from(server_files, file_path);

      char *live_hash = get_file_hash(project_name, file_path);
      char *stored_hash = curr_local->file_hash;
      char *server_hash = curr_server == NULL ? "" : curr_server->file_hash;

      bool is_same_file =
          curr_server == NULL ? false : strcmp(stored_hash, server_hash) == 0;
      bool is_modified = strcmp(stored_hash, live_hash) != 0;
      bool is_removed_client = get_file_from(local_files, file_path) == NULL;
      bool is_removed_server =
          curr_server == NULL ? true
                              : get_file_from(server_files, file_path) == NULL;

      // Delete case
      if (is_removed_client) {
        // Append 'D <file/path> <server's hash>' to .Commit
        dprintf(commit_fd, "D %s %s\n", file_path, server_hash);
        printf("D %s %s\n", file_path, server_hash);
      }
      // Modify Case
      else if (is_same_file && is_modified) {
        // Append 'M <file/path> <server's hash>' to .Commit
        dprintf(commit_fd, "M %s %s\n", file_path, server_hash);
        printf("M %s %s\n", file_path, server_hash);
      }
      // Add Case
      else if (is_removed_server) {
        // Append 'A <file/path> <server's hash>' to .Commit
        dprintf(commit_fd, "A %s %s\n", file_path, live_hash);
        printf("A %s %s\n", file_path, live_hash);
      }

      // Conflict case
      int server_version = curr_server == NULL ? -1 : curr_server->file_version;
      int local_version = curr_local->file_version;
      if (server_version > local_version ||
          (server_version == local_version && !is_same_file)) {
        printf(BRED "[Commit Error]" RESET " Update your local files first\n");
        return;
      }

      curr_local = curr_local->next;
    }

    close(commit_fd);

    // Write incremented file versions to `.Manifest`
    manifest_write(project_name, local_manifest);
  }

  // 2) Send Commit
  {
    // Add hash to `.Commit` filename to uniquely identify the commit
    // (`.Commit_<commit_hash>`)
    char *commit_hash = get_file_hash(project_name, ".Commit");
    char commit_filepath[1000] = {0};
    sprintf(commit_filepath, ".Commit_%s", commit_hash);

    FileList *commit_file = filelist_readfile(project_name, ".Commit");
    commit_file->file_path = commit_filepath;

    Request *commit_request = request_new();
    commit_request->command_name = "commit";
    commit_request->project_name = project_name;
    commit_request->filelist = commit_file;

    Response *commit_response = client_send(commit_request);
    if (commit_response->status_code < 0) {
      printf(BRED "[Commit Error]" RESET " %s\n", commit_response->message);
      remove(commit_path);
      return;
    }

    printf(BWHT "[Commit]" RESET " Successfully commited!\n");
  }
}

Response *commit_server(Request *request) {
  // Write commit to commits/<project_name>/.Commit_<commit_hash>
  char commit_directory[1000] = {0};
  sprintf(commit_directory, "commits/%s", request->project_name);

  mkdir(commit_directory, 0777);

  FileList *commit_file = request->filelist;
  filelist_write(commit_directory, commit_file);

  Response *response = response_new();
  response->message = "Successfully created .Commit";

  return response;
}