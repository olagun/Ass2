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
  if (!file_exists(file_path)) {
    printf("Add Error: Can't add a file that doesn't exist.");
    return;
  }

  FileList* filelist = filelist_new();
  filelist->file_path = file_path;
  FileList* file_with_bytes = filelist_readbytes(project_name, filelist);

  // https://rosettacode.org/wiki/SHA-256#C
  unsigned char* file_hash =
      SHA256((const unsigned char*)file_with_bytes->file_bytes,
             file_with_bytes->file_size, 0);

  int i = 0;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    printf("%02x\n", file_hash[i]);
  }
}
