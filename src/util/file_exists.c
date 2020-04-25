#include <stdbool.h>
#include <sys/stat.h>

// https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
bool file_exists(char* filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}