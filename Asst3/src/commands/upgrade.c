#include "src/commands/upgrade.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
#include "src/commands/remove.h"
#include "src/filelist.h"
#include "src/read.h"
#include "src/util/color.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_server_manifest.h"

void upgrade_client(char *project_name) {
  char conflict_path[1000] = {0};
  sprintf(conflict_path, "%s/.Conflict", project_name);

  char update_path[1000] = {0};
  sprintf(update_path, "%s/.Update", project_name);

  if (!directory_exists(project_name)) {
    printf(BRED "[Upgrade Error]" RESET
                " Project '%s' doesn't exist on the server\n",
           project_name);
    return;
  }

  if (file_exists(conflict_path)) {
    printf(BRED "[Upgrade Error]" RESET
                "First resolve all the conflicts, and then update\n");
    return;
  }

  if (!file_exists(update_path)) {
    printf(BRED "[Upgrade Error]" RESET " Run `.Update` first\n");
    return;
  }

  Manifest *client_manifest = manifest_read(project_name);
  Manifest *server_manifest = get_server_manifest(project_name);
  FileList *update_files = read_update_file(update_path);
  remove(update_path);

  if (client_manifest == NULL) {
    printf(BRED "[Upgrade Error]" RESET
                " Could not read the client manifest\n");
    return;
  }

  if (server_manifest == NULL) {
    printf(BRED "[Upgrade Error]" RESET
                " Could not read the server manifest\n");
    return;
  }

  if (update_files == NULL) {
    printf(BWHT "[Upgrade]" RESET " Up To Date\n");
    return;
  }

  FileList *client_files = client_manifest->filelist;
  FileList *server_files = server_manifest->filelist;
  FileList *update_item = update_files;

  while (update_item != NULL) {
    printf("%c %s %s\n", update_item->file_status, update_item->file_path,
           update_item->file_hash);

    char full_path[1000] = {0};
    sprintf(full_path, "%s/%s", project_name, update_item->file_path);

    FileList *server_file = get_file_from(server_files, update_item->file_path);
    FileList *client_file = get_file_from(client_files, update_item->file_path);

    if (server_file == NULL && update_item->file_status != 'D') {
      printf(BRED "[Upgrade Error]" RESET
                  " While modifying, file '%s' was not found in the server "
                  ".Manifest \n",
             update_item->file_path);
      return;
    }

    if (client_file == NULL && update_item->file_status != 'A') {
      printf(BRED "[Upgrade Error]" RESET
                  " While modifying, file '%s' was not found in the client "
                  ".Manifest \n",
             update_item->file_path);
      return;
    }

    // Delete case
    if (update_item->file_status == 'D') {
      client_files = filelist_remove(client_files, update_item->file_path);
    }

    // Modify case
    if (update_item->file_status == 'M') {
      int fd = creat(full_path, 0777);
      write(fd, server_file->file_bytes, server_file->file_size);
      close(fd);

      // Modify client manifest files
      client_file->file_hash = server_file->file_hash;
      client_file->file_version = server_file->file_version;
    }

    // Add case
    if (update_item->file_status == 'A') {
      int fd = creat(full_path, 0777);
      write(fd, server_file->file_bytes, server_file->file_size);
      close(fd);

      // Make copy of server file
      // (`get_file_from` returns a list not a node)
      FileList *file = filelist_new();
      file->file_path = server_file->file_path;
      file->file_bytes = server_file->file_bytes;
      file->file_size = server_file->file_size;
      file->file_hash = server_file->file_hash;

      client_files = filelist_append(client_files, file);
    }

    update_item = update_item->next;
  }

  client_manifest->filelist = client_files;
  manifest_write(project_name, client_manifest);
}

// Turn .Update file into `FileList`
FileList *read_update_file(char *update_path) {
  int fd = open(update_path, O_RDONLY, 0777);

  FileList *filelist = NULL;
  char *line;
  while (strcmp(line = read_line(fd), "") != 0) {
    FileList *item = filelist_new();

    item->file_path = calloc(strlen(line), sizeof(char));
    item->file_hash = calloc(strlen(line), sizeof(char));

    // Scan in <file_status> <file_path> <file_hash> from line
    sscanf(line, "%c %s %s", &item->file_status, item->file_path,
           item->file_hash);

    // Append item to filelist
    filelist = filelist_append(filelist, item);
  }

  close(fd);
  return filelist;
}
