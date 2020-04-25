#ifndef FILELIST_H
#define FILELIST_H

// Linked list of files
typedef struct FileList {
  char* file_path;
  int file_size;     // Number of bytes in file
  char* file_bytes;  // Contents of file

  // Manifest
  int file_version;
  int file_modified;
  char* file_hash;

  struct FileList* next;
} FileList;

FileList* filelist_new();
int filelist_size(FileList* list);

// Reads files bytes into filelist
FileList* filelist_readbytes(char* project_name, FileList* filelist);

// Writes filelist into project folder
// Creates new files, verwrites existing files, creates necessary
// directories
void filelist_write(char* project_name, FileList* filelist);

// Append a file list item to a file list
FileList* filelist_append(FileList* filelist, FileList* item);

#endif