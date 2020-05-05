#include "src/commands/push.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
#include "src/compression.h"
#include "src/filelist.h"
#include "src/read.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"

// Request status codes
int check_commit = 0;  // 1
int push_commit = 1;   // 2

void push_client(char* project_name) {
  // 1) Check if commit exists on the server.
  Request* request = request_new();
  request->command_name = "push";
  request->project_name = project_name;
  request->status_code = check_commit;

  // Instead of sending the entire .Commit file, I just send the hash of the
  // commit file. On the server, I check if the sent hash matches with a commit
  // stored on the server.
  char* commit_hash = get_file_hash(project_name, ".Commit");
  request->message = commit_hash;

  Response* response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

  // 2) Push commit files.
  Manifest* manifest = manifest_read(project_name);

  request = request_new();
  request->command_name = "push";
  request->message = commit_hash;
  request->project_name = project_name;
  request->status_code = push_commit;
  request->project_version = manifest->project_version + 1;

  // Read in contents
  FileList* files = filelist_readbytes(project_name, manifest->filelist);
  // Increment version
  files = filelist_increment_version(files);
  // Update hash
  files = filelist_update_hash(project_name, files);

  request->filelist = files;

  response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

  // Update client manfiest
  manifest->project_version++;
  manifest_write(project_name, manifest);

  // Delete .Commit
  char* commit_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(commit_path, "%s/.Commit", project_name);
  remove(commit_path);
}

Response* push_server(Request* request) {
  char* project_name = request->project_name;

  char* project_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(project_path, "projects/%s", project_name);

  if (request->status_code == check_commit) {
    // FAIL if project doesn't exist
    if (!directory_exists(project_path)) {
      Response* response = response_new();
      response->message = "Project doesn't exist";
      response->status_code = -1;
      return NULL;
    }

    // FAIL if commit file doesn't exist
    char* commit_hash = request->message;

    char* commit_file_path = calloc(strlen(commit_hash) + 50, sizeof(char));
    sprintf(commit_file_path, "commits/%s/.Commit_%s", project_name,
            commit_hash);

    bool commit_exist = file_exists(commit_file_path);
    if (!commit_exist) {
      Response* response = response_new();
      response->message =
          "Commit doesn't exist on the server. `commit` then `push`.";
      response->status_code = -1;
      return response;
    }

    Response* response = response_new();
    response->message = "Ok! Send me your changes now!";
    return response;
  }

  if (request->status_code == push_commit) {
    char* commit_hash = request->message;

    char project_path[1000] = {0};
    sprintf(project_path, "projects/%s", project_name);

    char commit_file_path[1000] = {0};
    sprintf(commit_file_path, "commits/%s/.Commit_%s", project_name,
            commit_hash);

    // Create history folder for project
    char history_path[1000] = {0};
    sprintf(history_path, "history/%s", project_name);
    mkdir(history_path, 0777);

    // Create history folder for version
    sprintf(history_path, "history/%s/%d", project_name,
            request->project_version - 1);
    mkdir(history_path, 0777);

    // Write compressed current project files to history
    char history_archive[1000] = {0};
    sprintf(history_archive, "history/%s/%d/.History", project_name,
            request->project_version - 1);

    char manifest_archive[1000] = {0};
    sprintf(manifest_archive, "history/%s/%d", project_name,
            request->project_version - 1);

    int write_fd = creat(history_archive, 0777);

    Manifest* project_manifest = manifest_read(project_path);
    manifest_write(manifest_archive, project_manifest);

    FileList* list = project_manifest->filelist;
    list = filelist_readbytes(project_path, list);
    write_filelist_compressed(write_fd, list);

    close(write_fd);

    /* Remove old project folder, create project folder and write files and
     * manifest */

    // Remove old project
    char project_sys[1000] = {0};
    sprintf(project_sys, "rm -rf projects/%s", project_name);
    system(project_sys);

    // Create project folder and write files and manifest
    mkdir(project_path, 0777);
    filelist_write(project_path, request->filelist);
    Manifest* manifest = manifest_new();
    manifest->project_version = request->project_version;
    manifest->filelist = request->filelist;
    manifest_write(project_path, manifest);

    // Move commit file
    char mv_commit[1000] = {0};
    sprintf(mv_commit, "mv commits/%s/.Commit_%s history/%s/%d/.Commit",
            project_name, commit_hash, project_name,
            request->project_version - 1);
    system(mv_commit);

    Response* response = response_new();
    response->message = "Awesome, committed!";
    response->project_version = manifest->project_version;
    return response;
  }

  return NULL;
}