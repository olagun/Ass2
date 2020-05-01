#include "src/manifest.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/read.h"

Manifest* manifest_new() { return calloc(1, sizeof(Manifest)); }

// Reads the manifest from a project path
Manifest* manifest_read(char* project_path) {
  // Open manifest
  char* manifest_path = calloc(strlen(project_path) + 50, sizeof(char));
  sprintf(manifest_path, "%s/.Manifest", project_path);
  int manifest_fd = open(manifest_path, O_RDONLY, 0777);
  if (manifest_fd < 0) {
    printf("[Manifest Error] Could not read .Manifest at %s\n", project_path);
    return NULL;
  }

  // Turn .Manifest file into `Manifest`
  Manifest* manifest = manifest_new();
  manifest->project_version = atoi(read_line(manifest_fd));

  char* line;
  while (strcmp(line = read_line(manifest_fd), "\0") != 0) {
    FileList* item = filelist_new();
    item->file_path = calloc(strlen(line), sizeof(char));
    item->file_hash = calloc(strlen(line), sizeof(char));

    // Scan in <file_path> <file_version> <file_hash> <file_removed> from line
    sscanf(line, "%s %d %s %d", item->file_path, &item->file_version,
           item->file_hash, &item->file_removed);

    // Append item to list
    manifest->filelist = filelist_append(manifest->filelist, item);
  }
  close(manifest_fd);

  // After having read .Manifest, Now read the project files.
  manifest->filelist = filelist_readbytes(project_path, manifest->filelist);
  return manifest;
}

// Write manifest to project path. Overwrites if already exists
void manifest_write(char* project_path, Manifest* manifest) {
  // Error if project path doesn't exist
  int missing_folder = opendir(project_path) == NULL;
  if (missing_folder) {
    printf("Can't write manifest to non-existent project\n");
    return;
  }

  // Open manifest
  char* manifest_path = calloc(strlen(project_path) + 50, sizeof(char));
  sprintf(manifest_path, "%s/.Manifest", project_path);
  int manifest_fd = creat(manifest_path, 0777);

  // Write project version
  dprintf(manifest_fd, "%d\n", manifest->project_version);

  // Write every file except for `.Manifest`
  FileList* item = manifest->filelist;
  while (item != NULL) {
    // Write <file_path> <file_version> <file_hash> <file_removed> to line
    dprintf(manifest_fd, "%s %d %s %d\n", item->file_path, item->file_version,
            item->file_hash, item->file_removed);

    item = item->next;
  }
  close(manifest_fd);
}
