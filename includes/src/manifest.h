#include "src/filelist.h"
#include "src/token.h"

#ifndef MANIFEST_H
#define MANIFEST_H

typedef struct Manifest {
  int project_version;
  FileList* filelist;
} Manifest;

Manifest* manifest_new();

// Reads in the manifest from project
Manifest* manifest_read(char* project_path);

#endif