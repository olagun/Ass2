#include <fcntl.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/manifest.h"
#include "src/util/file_exists.h"

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

  unsigned char* raw_hash =
      SHA256((unsigned const char*)file_bytes, file_size, 0);
  char* hex_hash = calloc(SHA256_DIGEST_LENGTH * 2 + 1, sizeof(char));

  int i = 0;
  for (; i < SHA256_DIGEST_LENGTH; i++) {
    // Print two hex chars (0x00) for every byte
    sprintf(hex_hash + 2 * i, "%02x", raw_hash[i]);
  }

  filelist->file_hash = hex_hash;

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
