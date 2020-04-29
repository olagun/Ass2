#include <openssl/sha.h>
//#include <./Documents/Code/anaconda3/pkgs/openssl-1.1.1d-h1de35cc_2/include/openssl/sha.h>
//#include </usr/local/opt/openssl/include/openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>

#include "src/filelist.h"

// https://rosettacode.org/wiki/SHA-256#C
char* get_file_hash(char* project_name, char* file_path) {
  FileList* filelist = filelist_new();
  filelist->file_path = file_path;
  filelist = filelist_readbytes(project_name, filelist);

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

  return hex_hash;
}