#include <sys/stat.h>

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
int get_file_size(char* filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}
