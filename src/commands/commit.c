#include "src/commands/commit.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "src/testing.h"
#include "src/client.h"
#include "src/commands/commands.h"
#include "src/filelist.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"

void commit_client(char *project_name) {
  // Update path
  char *update_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(update_path, "%s/.Update", project_name);

  // Fail if has non-empty `.Update` file
  bool has_nonempty_update =
      file_exists(update_path) && get_file_size(update_path) > 0;
  if (has_nonempty_update) {
    printf(
        "[Commit Error] Contains non-empty `.Update` file. `upgrade` before "
        "commiting.\n");
    return;
  }

  // Conflict path
  char *conflict_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(conflict_path, "%s/.Conflict", project_name);

  // Fail if has `.Conflict` file
  bool has_confict = file_exists(conflict_path);
  if (has_confict) {
    printf(
        "[Commit Error] Contains `.Conflict` file. Resolve conflict and "
        "`upgrade` before commiting.\n");
    return;
  }

  // Commit path
  char *commit_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(commit_path, "%s/.Commit", project_name);

  // 1) Request manifest from server
  {
    Request *request = request_new();
    request->command_name = "commit";
    request->project_name = project_name;
    request->status_code = get_manifest;

    Response *response = client_send(request);
    if (response->status_code < 0) {
      printf("%s\n", response->message);
      return;
    }

    Manifest *local_manifest = manifest_read(project_name);

    // Check if manifest versions match
    if (response->project_version != local_manifest->project_version) {
      printf("[Commit Error] Update your local files first.\n");
      return;
    }

    // Create commit file
    int commit_fd = creat(commit_path, 0777);
    if (commit_fd < 0) {
      printf("[Commit Error] Could not create `.Commit`.\n");
      return;
    }

    FileList *local_files = local_manifest->filelist;
    FileList *server_files = response->filelist;

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
      bool is_removed_client = curr_local->file_removed;
      bool is_removed_server =
          curr_server == NULL ? true : curr_server->file_removed;

      // Delete case
      if (is_removed_client) {
        // Increment file version (This does nothing because the file is removed
        // later)
        curr_local->file_version++;

        // Append 'D <file/path> <server's hash>' to .Commit
        dprintf(commit_fd, "D %s %s\n", file_path, server_hash);
        printf("D %s %s\n", file_path, server_hash);
      }
      // Modify Case
      else if (is_same_file && is_modified) {
        // Increment file version
        curr_local->file_version++;

        // Append 'M <file/path> <server's hash>' to .Commit
        dprintf(commit_fd, "M %s %s\n", file_path, server_hash);
        printf("M %s %s\n", file_path, server_hash);
      }
      // Add Case
      else if (is_removed_server) {
        // Increment file version
        curr_local->file_version++;

        // Append 'A <file/path> <server's hash>' to .Commit
        dprintf(commit_fd, "A %s %s\n", file_path, live_hash);
        printf("A %s %s\n", file_path, live_hash);
      }

      // Conflict case
      int server_version = curr_server == NULL ? -1 : curr_server->file_version;
      int local_version = curr_local->file_version;
      if (server_version > local_version ||
          (server_version == local_version && !is_same_file)) {
        printf("[Commit Error] Update your local files first.\n");
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
    // Create `.Commit` filelist to send over
    FileList *commit_file = filelist_readfile(project_name, ".Commit");

    // Add hash to `.Commit` filename to uniquely identify the commit
    // (`.Commit_<commit_hash>`)
    char *commit_hash = get_file_hash(project_name, ".Commit");
    char *commit_filepath = calloc(strlen(commit_hash) + 50, sizeof(char));
    sprintf(commit_filepath, ".Commit_%s", commit_hash);
    commit_file->file_path = commit_filepath;

    // Send `.Commit_<commit_hash>`
    Request *commit_request = request_new();
    commit_request->command_name = "commit";
    commit_request->project_name = project_name;
    commit_request->status_code = send_commit;
    commit_request->filelist = commit_file;

    Response *commit_response = client_send(commit_request);
    if (commit_response->status_code < 0) {
      printf("[Commit Error] %s\n", commit_response->message);
    }

    // remove(commit_path);
    if (!TESTING) {
    printf("[Commit] Successfully commited!\n");
    }

  }
}

Response *commit_server(Request *request) {
  // Respond to `get_manifest`
  if (request->status_code == get_manifest) {
    char *project_path =
        calloc(strlen(request->project_name) + 50, sizeof(char));
    sprintf(project_path, "projects/%s", request->project_name);

    // Fail if project does not exist on the server
    if (!directory_exists(project_path)) {
      Response *response = response_new();
      response->status_code = -1;
      response->message =
          "[Commit Error] Project name does not exist on the server.";
      return response;
    }

    // Fetch the server's manifest file
    Manifest *manifest = manifest_read(project_path);

    if (manifest == NULL) {
      Response *response = response_new();
      response->status_code = -1;
      response->message = "[Manifest Error] Could not read .Manifest";
      return response;
    }

    Response *response = response_new();
    response->message = "Successfully read .Manifest";
    response->project_version = manifest->project_version;
    response->filelist = manifest->filelist;
    return response;
  }

  // Respond to `send_commit`
  if (request->status_code == send_commit) {
    
    // Write commit to commits/<project_name>/.Commit_<commit_hash>
    char *commit_directory =
        calloc(strlen(request->project_name) + 50, sizeof(char));
    sprintf(commit_directory, "commits/%s", request->project_name);

    mkdir(commit_directory, 0777);

    FileList *commit_file = request->filelist;
    filelist_write(commit_directory, commit_file);

    Response *response = response_new();
    response->message = "Successfully created .Commit";
    return response;
  }

  // Ooops. This should never happen
  return NULL;
}