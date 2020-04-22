#include "../filelist/filelist.h"
#include "../token/token.h"

#ifndef MANIFEST_H
#define MANIFEST_H

typedef struct Manifest {
  int project_version;
  FileList* filelist;
} Manifest;

Manifest* manifest_new();

// Reads in the manifest from specificed project
Manifest* manifest_read(char* project);

// TODO:
// Compares two manifests. Used in "commit" and "update"
// outputs a linked list of strings containng changes
// ex: "A <file_path> <server_hash>" -> "D <file_path> <server_hash>" -> ...
Token* manifest_compare(Manifest* a, Manifest* b);

#endif