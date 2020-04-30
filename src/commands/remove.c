#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/manifest.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
//include <openssl/md5.h>

int remove_client(char *project_name, char *file_path)
{

    // Concatentate project name and file path
    int full_path_len = strlen(file_path) + strlen(project_name);
    char* full_path = calloc(full_path_len + 50, sizeof(char));
    sprintf(full_path, "%s/%s", project_name, file_path);

    if (!file_exists(full_path)) {
        printf("Remove Error: Can't remove a file that doesn't exist.\n");
        return -1;
    }

    // //the file exists, delete the file
    // int status;
    // status = remove(full_path);
    // if (status == 0)
    //     printf("%s file deleted successfully.\n", file_path);
    // else
    // {
    //     printf("Unable to delete file %s\n", file_path);
    //     perror("Following error occurred");
    // }

    // Read in project's `.Manifest`
    Manifest *manifest = manifest_read(project_name);

    FileList *curr = manifest->filelist;
    FileList *prev = NULL;
    while (curr != NULL )
    {
        // If it's already there, then set file_removed to 1  and leave
        if (strcmp(curr->file_path, file_path) == 0)
        {
            curr->file_removed = 1;

            // Write to client .Manifest
            manifest_write(project_name, manifest);
            return 0; // no error
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Unable to delete file %s from .Manifest.\n", file_path);
    return -1;
}
