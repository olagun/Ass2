
#include "src/commands/history.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
#include "src/filelist.h"
#include "src/read.h"
#include "src/token.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"

void history_client(char* project_name) {
  Request* request = request_new();
  request->command_name = "history";
  request->project_name = project_name;

  Response* response = client_send(request);
  if (response->status_code < 0) {
    printf("%s\n", response->message);
    return;
  }

  char* history_file = response->message;
  printf("%s\n", history_file);
}

Response* history_server(Request* request) {
  char* project_name = request->project_name;

  char* history_path = calloc(strlen(project_name) + 50, sizeof(char));
  sprintf(history_path, "history/%s", project_name);

   if (!directory_exists(history_path)) {
    Response* response = response_new();
    response->status_code = -1;
    response->message = "[History Error] Project doesn't exist in history";
    return response;
  }

  DIR* history_dir = opendir(history_path);

  Token* head = NULL;
  struct dirent* entry;
  while ((entry = readdir(history_dir)) != NULL) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      char* project_version = entry->d_name;
      head = token_append(head, token_new(project_version));
      head = token_append(head, token_new("\n"));

      char* commit_file_path = calloc(strlen(project_name) + 50, sizeof(char));
      sprintf(commit_file_path, "history/%s/%s/.Commit", project_name,
              project_version);
      int fd = open(commit_file_path, O_RDONLY, 0777);
      free(commit_file_path);

      char* line;
      while (*(line = read_line(fd)) != '\0') {
        head = token_append(head, token_new(line));
        head = token_append(head, token_new("\n"));
      }
      
      head = token_append(head, token_new("\n"));
    }
  }

  closedir(history_dir);

  char* history_file = token_string(head);

  Response* response = response_new();
  response->message = history_file;

  return response;
}