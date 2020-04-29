#include "src/commands/commands.h"
#include "src/commands/commit.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/client.h"
#include "src/filelist.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"

// Status codes moved to commands.h
//const int get_manifest = 0;
//const int send_commit = 1;

// TODO(Sam): Do Manifest files have to be sorted? If they're not
// tampered with they should be in the same order right?
void commit_client(char *project_name)
{
  char *update_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(update_path, "projects/%s/.Update", project_name);

  char *conflict_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(conflict_path, "projects/%s/.Conflict", project_name);

  if ((file_exists(update_path) && get_file_size(update_path) > 0) ||
      file_exists(conflict_path))
  {
    printf("Contains .Update or .Conflict file. Could not be commit\n");
    return;
  }

  Request *request = request_new();
  request->command_name = "commit";
  request->project_name = project_name;
  request->status_code = get_manifest;

  Response *response = client_send(request);

  if (response->status_code < 0)
  {
    printf("%s\n", response->message);
    return;
  }

  Manifest *local_manifest = manifest_read(project_name);

  // Exit if the versions don't match
  if (response->project_version != local_manifest->project_version)
  {
    printf("Manifest's don't match. Update your local files first.\n");
    return;
  }

  FileList *local_files = local_manifest->filelist;
  FileList *server_files = response->filelist;

  // Create `.Commit`
  char *commit_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(commit_path, "%s/.Commit", project_name);
  int commit_fd = creat(commit_path, 0777);

  while (local_files != NULL && server_files != NULL)
  {
    char *live_hash = get_file_hash(project_name, local_files->file_path);
    char *stored_hash = local_files->file_hash;
    char *server_hash = server_files->file_hash;

    // Modify code case
    if (!local_files->file_removed && strcmp(stored_hash, server_hash) == 0 &&
        strcmp(stored_hash, live_hash) != 0)
    {
      dprintf(commit_fd, "M %s %s\n", local_files->file_path, server_hash);

      // Increment file version
      local_files->file_version++;
    }

    // Delete code case
    if (local_files->file_removed)
    {
      dprintf(commit_fd, "D %s %s\n", local_files->file_path, server_hash);
      // TODO(Sam): Instructions say to increment the file version here when
      // that is impossible.
    }

    local_files = local_files->next;
    server_files = server_files->next;
  }

  // Add code case
  // TODO(Sam): Is adding all remaining files correct? Come back to this
  while (local_files != NULL)
  {
    // TODO(Sam): The instructions say to write <server_hash> even though this
    // is clearly impossible.
    dprintf(commit_fd, "A %s %s\n", local_files->file_path,
            local_files->file_hash);
    local_files = local_files->next;
  }

  close(commit_fd);

  // Write to manifest with incremented file versions
  manifest_write(project_name, local_manifest);

  FileList *commit_file = filelist_new();
  commit_file->file_path = ".Commit";
  commit_file = filelist_readbytes(project_name, commit_file);

  // Add hash to uniquely identify the commit
  char *commit_hash = get_file_hash(project_name, ".Commit");
  char *commit_filepath = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(commit_filepath, ".Commit_%s", commit_hash);
  commit_file->file_path = commit_filepath;

  Request *commit_request = request_new();
  commit_request->command_name = "commit";
  commit_request->status_code = send_commit;
  commit_request->filelist = commit_file;

  Response *commit_response = client_send(commit_request);
}

Response *commit_server(Request *request)
{
  if (request->status_code == get_manifest)
  {
    char *full_path = calloc(strlen(request->project_name) + 50, sizeof(char));
    sprintf(full_path, "projects/%s", request->project_name);

    if (!directory_exists(full_path))
    {
      Response *response = response_new();
      response->status_code = -1;
      response->message = "Project doesn't exist on the server";
      return response;
    }
  }

  if (request->status_code == send_commit)
  {
    FileList *commit_file = request->filelist;
    filelist_write("commits", commit_file);
  }

  return NULL;
}