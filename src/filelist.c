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
#include "src/util/get_file_size.h"

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

FileList* filelist_readfile(char* project_name, char* file_path) {
  FileList* file = filelist_new();
  file->file_path = file_path;
  file = filelist_readbytes(project_name, file);
  return file;
}

// Reads files bytes into filelist
FileList* filelist_readbytes(char* project_name, FileList* filelist) {
  if (filelist == NULL) return NULL;

  // Concatenate project_name to file_path
  char* full_path = calloc(strlen(filelist->file_path) + 50, sizeof(char));
  sprintf(full_path, "%s/%s", project_name, filelist->file_path);

  // Write to file
  int fd = open(full_path, O_RDONLY, 0777);
  filelist->file_size = get_file_size(full_path);
  filelist->file_bytes = read_nbytes(fd, filelist->file_size);
  close(fd);

  // Recurse on the rest of the list
  filelist->next = filelist_readbytes(project_name, filelist->next);

  return filelist;
}

// Writes filelist into project folder
// Creates new files, verwrites existing files, creates necessary
// directories
void filelist_write(char* project_name, FileList* filelist) {
  if (filelist == NULL) return;

  // Error if project path doesn't exist
  int missing_folder = opendir(project_name) == NULL;
  if (missing_folder) {
    printf("Can't write files to a non-existent project\n");
    return;
  }

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

FileList* filelist_append(FileList* filelist, FileList* item) {
  if (filelist == NULL) return item;
  filelist->next = filelist_append(filelist->next, item);
  return filelist;
}

FileList *get_file_from(FileList *file_list, char *in_file_path)
{
    //returns a pointer to the file list if the file name matches
    FileList *cur_file = file_list;
    while (cur_file != NULL)
    {
        if (strcmp(cur_file->file_path, in_file_path) == 0)
        {
            return cur_file;
        }
        cur_file = cur_file->next;
    }
    return NULL;
}

