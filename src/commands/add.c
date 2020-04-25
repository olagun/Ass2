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
    printf("Add Error: Can't add a file that doesn't exist.");
    return;
  }

  // Read in project's `.Manifest`
  Manifest* manifest = manifest_read(project_name);

  // Append the file
  FileList* curr = manifest->filelist;
  FileList* prev = NULL;
  while (curr != NULL) {
    // If it's already there, increment the file version and leave
    if (strcmp(curr->file_path, file_path) == 0) {
      curr->file_version++;

      // Write to project's `.Manifest`
      manifest_write(project_name, manifest);
      return;
    }
    prev = curr;
    curr = curr->next;
  }

  // Append the file to manifest
  FileList* filelist = filelist_new();
  filelist->file_path = file_path;
  filelist->file_hash = get_file_hash(project_name, file_path);
  filelist->file_version = 0;
  filelist->file_removed = 0;

  if (prev) {
    prev->next = filelist;
  } else {
    manifest->filelist = filelist;
  }

  // Write to project's `.Manifest`
  manifest_write(project_name, manifest);

  // TODO:
  // Make `void manifest_free(Manifest* manifest)`
}
