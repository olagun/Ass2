#include <fcntl.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/manifest.h"

// https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
bool file_exists(char* filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

void add_client(char* project_name, char* file_path) {
  // Concatentate project name and file path
  int full_path_len = strlen(file_path) + strlen(project_name);
  char* full_path = calloc(full_path_len + 50, sizeof(char));
  sprintf(full_path, "%s/%s", project_name, file_path);

  if (!file_exists(full_path)) {
    printf("Add Error: Can't add a file that doesn't exist.");
    return;
  }

  // Read in the file
  FileList* filelist = filelist_new();
  filelist->file_path = file_path;
  filelist = filelist_readbytes(project_name, filelist);

  // Get the file hash
  // https://rosettacode.org/wiki/SHA-256#C
  char* file_bytes = filelist->file_bytes;
  int file_size = filelist->file_size;
  filelist->file_hash =
      (char*)SHA256((const unsigned char*)file_bytes, file_size, 0);

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
