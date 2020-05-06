#ifndef FILELIST_H
#define FILELIST_H

// Linked list of files
typedef struct FileList {
  char* file_path;
  int file_version;
  char* file_hash;
  int file_size;     // Number of bytes in file
  unsigned char* file_bytes;  // Contents of file
  char file_status;  // This is only for .Update & .Conflict files, and not in
                     // .Manifest

  struct FileList* next;
} FileList;

FileList* filelist_new();
int filelist_size(FileList* list);

FileList* filelist_remove(FileList* filelist, char* path);

// Reads files bytes into filelist
FileList* filelist_readbytes(char* project_name, FileList* filelist);

// Writes filelist into project folder
// Creates new files, overwrites existing files, creates necessary
// directories
void filelist_write(char* project_name, FileList* filelist);

FileList* filelist_readfile(char* project_name, char* file_path);

// Append a file list item to a file list
FileList* filelist_append(FileList* filelist, FileList* item);

// search for file in filelist
FileList* get_file_from(FileList* file_list, char* in_file_path);

FileList* filelist_increment_version(FileList* list);

FileList *filelist_update_hash(char *project_name, FileList *list);

#endif