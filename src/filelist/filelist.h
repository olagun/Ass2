#ifndef FILELIST_H
#define FILELIST_H

// Linked list of files
typedef struct FileList {
  char* file_path;   // File name
  int file_size;     // Number of bytes in file
  char* file_bytes;  // Contents of file

  // Manifest
  int file_version;
  char* file_hash;
  int file_hash_size;

  // Pointer to next file
  struct FileList* next;
} FileList;

FileList* filelist_new();
int filelist_size(FileList* list);

// Reads files contents into filelist
FileList* filelist_readbytes(char* project_name, FileList* filelist);

// Writes filelist into project folder
// Creates non-existing files, overwrites existing files, deletes no files
void filelist_write(char* project_name, FileList* filelist);

#endif