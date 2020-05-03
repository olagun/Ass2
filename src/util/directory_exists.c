#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

// https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
bool directory_exists(char* directory_path) {
  DIR* dir = opendir(directory_path);
  if (dir != NULL) return true;
  return ENOENT != errno;
}