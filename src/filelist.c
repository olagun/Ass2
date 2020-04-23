#include "src/filelist.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/manifest.h"
#include "src/read.h"

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
int get_file_size(char* filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}

// Returns index of the nth `delim`
int find_nth(char* str, char delim, int n) {
  int index = 0, len = strlen(str);

  for (; index < len; index++) {
    char curr_char = str[index];

    if (curr_char == delim) {
      if (n == 0) return index;
      n = n - 1;
    }
  }

  return -1;
}

FileList* filelist_new() { return calloc(1, sizeof(FileList)); }

int filelist_size(FileList* list) {
  if (list == NULL) return 0;
  return 1 + filelist_size(list->next);
}

// Reads files bytes into filelist
FileList* filelist_readbytes(char* project_name, FileList* filelist) {
  if (filelist == NULL) return NULL;

  // Concatenate project_name to file_path
  char* full_path = calloc(strlen(filelist->file_path) + 50, sizeof(char));
  sprintf(full_path, "%s/%s", project_name, filelist->file_path);

  // Write to file
  int fd = open(full_path, O_RDONLY, 0777);
  filelist->file_bytes = read_nbytes(fd, filelist->file_size);
  filelist->file_size = get_file_size(full_path);
  close(fd);

  // Recurse on the rest of the list
  filelist->next = filelist_readbytes(project_name, filelist->next);

  return filelist;
}

// Writes filelist into project folder
// Creates new files, verwrites existing files, creates necessary
// directories
void filelist_write(char* project_name, FileList* filelist) {
  // Add project folder (if it's not already there)
  int missing_folder = opendir(project_name) == NULL;
  if (missing_folder) mkdir(project_name, 0777);

  // Add every file
  FileList* item = filelist;
  while (item != NULL) {
    // Concatenate project_name to file_path
    char* full_path = calloc(strlen(item->file_path) + 50, sizeof(char));
    sprintf(full_path, "%s/%s", project_name, item->file_path);

    // TODO: Doesn't work with trailing slash
    // e.g."test/dir/text.txt/" vs. "test/dir/text.txt"

    // Add missing folders
    int n = 0, slash_index = 0;
    while ((slash_index = find_nth(full_path, '/', n)) > 0) {
      // Get string before slash
      char* before_slash = strndup(full_path, slash_index);

      // Add directory (if it's not already there)
      int missing_folder = opendir(before_slash) == NULL;
      if (missing_folder) mkdir(before_slash, 0777);

      // Find the next slash
      n = n + 1;
    }

    // After missing folders are added, create the file
    int file_fd = creat(full_path, 0777);
    write(file_fd, item->file_bytes, item->file_size);
    close(file_fd);

    item = item->next;
  }
}