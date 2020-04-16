//sendfile:2:9:folder/thing.txt21:8:folder/stuf.txt9:0f009fflll1l100JIAlz0&89*1H9s0

// ":"
// <command>:<project_name>:<serialized_file stuff>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>

// //https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm

// char* serialize(char* manifest_path) {
//    // ./Project/folder1/file.txt/
//   DIR *dir = opendir(pathname);

//   if (dir == NULL) {
//     // Unable to open as directory - must be a file.


//     return;
//   }

//   struct dirent *dp;
//   char path[1000];
//   while ((dp = readdir(dir)) != NULL) {
//     if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
//       strcpy(path, pathname);
//       strcat(path, "/");
//       strcat(path, dp->d_name);

//       listFilesRecursive(path);
//     }
//   }

//   closedir(dir);

  
//   return NULL;
// }

// char* deserialize(char* file_string) {
//     //char* project = strtok(pathname, ":");
//   // char* file_count = 
//   return NULL;
// }