#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/manifest.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"

void add_client(char* project_name, char* file_path) {
  // Concatentate project name and file path
  int full_path_len = strlen(file_path) + strlen(project_name);
  char* full_path = calloc(full_path_len + 50, sizeof(char));
  sprintf(full_path, "%s/%s", project_name, file_path);

  if (!file_exists(full_path)) {
    printf("Add Error: Can't add a file that doesn't exist.\n");
    return;
  }

  // Append the file to manifest
  FileList* file = filelist_new();
  file->file_path = file_path;
  file->file_hash = get_file_hash(project_name, file_path);

  // Append the file to manifest
  Manifest* manifest = manifest_read(project_name);
  manifest->filelist = filelist_append(manifest->filelist, file);

  // Write to manifest
  manifest_write(project_name, manifest);

  // TODO(Sam): Make `void manifest_free(Manifest* manifest)`
}
