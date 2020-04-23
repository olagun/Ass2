#include "src/manifest.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/read.h"

Manifest* manifest_new() { return calloc(1, sizeof(Manifest)); }

// Reads the manifest from a project path
Manifest* manifest_read(char* project_path) {
  // Open manifest
  char* manifest_path = calloc(strlen(project_path) + 50, sizeof(char));
  sprintf(manifest_path, "%s/.Manifest", project_path);
  int manifest_fd = open(manifest_path, O_RDONLY, 0777);

  // Turn manifest file into `Manifest`
  Manifest* manifest = manifest_new();
  manifest->project_version = atoi(read_line(manifest_fd));

  char* line;
  while (strcmp(line = read_line(manifest_fd), "\0") != 0) {
    FileList* item = filelist_new();
    item->file_path = calloc(strlen(line), sizeof(char));
    item->file_hash = calloc(strlen(line), sizeof(char));

    // Scan in <file_path> <file_version> <file_hash> from line
    sscanf(line, "%s %d %s", item->file_path, &item->file_version,
           item->file_hash);

    // Prepend file to list
    item->next = manifest->filelist;
    manifest->filelist = item;
  }
  close(manifest_fd);

  // Add `.Manifest` to the manifest file list
  // (Because `.Manifest` isn't stored in `.Manifest`)
  FileList* item = filelist_new();
  item->file_path = ".Manifest";

  // Prepend `.Manifest` to list
  item->next = manifest->filelist;
  manifest->filelist = item;

  return manifest;
}