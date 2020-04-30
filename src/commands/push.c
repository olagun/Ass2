
#include "src/commands/push.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
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
  request->filelist = filelist_readbytes(project_name, manifest->filelist);

  response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

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

    char* commit_file_path = calloc(strlen(commit_hash) + 50, sizeof(char));
    sprintf(commit_file_path, "commits/%s/.Commit_%s", project_name,
            commit_hash);

    // Make history folder
    char* history_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(history_path, "history/%s", project_name);
    mkdir(history_path, 0777);

    // Copy files into history
    char* mv_project = calloc(strlen(project_name) + 50, sizeof(char));
    // TODO(Sam): Add hash to history
    sprintf(mv_project, "mv projects/%s history/%s/%d", project_name,
            project_name, request->project_version - 1);
    system(mv_project);

    char* project_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(project_path, "projects/%s", project_name);

    mkdir(project_path, 0777);
    filelist_write(project_path, request->filelist);

    // Move commit file
    char* mv_commit = calloc(strlen(commit_file_path) + 50, sizeof(char));
    sprintf(mv_commit, "mv commits/%s/.Commit_%s history/%s/%d/.Commit",
            project_name, commit_hash, project_name,
            request->project_version - 1);
    system(mv_commit);

    Manifest* manifest = manifest_new();
    manifest->project_version = request->project_version;
    manifest->filelist = request->filelist;
    manifest_write(project_path, manifest);

    Response* response = response_new();
    response->message = "Awesome, committed!";
    return response;

    // All files except for `.Manifest`
    // Strategy: Make a copy of the current directory
    // Delete all files
    // Write all files

    // system call to copy files
    // delete direcotry
    // recreate nd write all files
    // make a new manifest file

    // Add commt to thistory file nad move
    // FileList* filest =
  }

  return NULL;
}