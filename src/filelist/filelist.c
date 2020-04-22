#include "filelist.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../manifest/manifest.h"
#include "../read/read.h"

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
int get_file_size(char* filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}

int find_nth_index(char* str, char delim, int n) {
  int index = 0;
  int len = strlen(str);

  while (index < len) {
    char curr_char = str[index];

    if (curr_char == delim) {
      if (n == 0) {
        return index;
      }

      n = n - 1;
    }

    index++;
  }

  return -1;
}

FileList* filelist_new() { return calloc(1, sizeof(FileList)); }

int filelist_size(FileList* list) {
  if (list == NULL) return 0;
  return 1 + filelist_size(list->next);
}

// Reads files contents into filelist
FileList* filelist_readbytes(char* project_name, FileList* filelist) {
  if (filelist == NULL) return NULL;

  char* pathname = calloc(strlen(filelist->file_path) + 50, sizeof(char));
  sprintf(pathname, "%s/%s", project_name, filelist->file_path);

  int fd = open(pathname, O_RDONLY, 0777);
  filelist->file_size = get_file_size(pathname);
  filelist->file_bytes = read_nbytes(fd, filelist->file_size);
  close(fd);

  filelist->next = filelist_readbytes(project_name, filelist->next);

  return filelist;
}

// Writes filelist into project folder
// Creates non-existing files, overwrites existing files, deletes no files
void filelist_write(char* project_name, FileList* filelist) {
  int missing_folder = opendir(project_name) == NULL;
  if (missing_folder) {
    mkdir(project_name, 0777);
  }

  FileList* item = filelist;
  while (item != NULL) {
    char* full_path = calloc(strlen(item->file_path) + 50, sizeof(char));
    sprintf(full_path, "%s/%s", project_name, item->file_path);

    // TODO: Doesn't work with trailing slash
    // e.g."test/dir/text.txt/" vs. "test/dir/text.txt"

    // Go through and add missing folders
    int n = 0, slash_index = 0;
    while ((slash_index = find_nth_index(full_path, '/', n)) > 0) {
      char* string_until_slash = strndup(full_path, slash_index);

      int missing_folder = opendir(string_until_slash) == NULL;
      if (missing_folder) {
        mkdir(string_until_slash, 0777);
      }

      n = n + 1;
    }

    int file_fd = creat(full_path, 0777);
    write(file_fd, item->file_bytes, item->file_size);
    close(file_fd);

    item = item->next;
  }
}