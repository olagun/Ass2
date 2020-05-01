#include "src/commands/push.h"
#include "src/commands/upgrade.h"
#include "src/commands/commands.h"
#include "src/commands/remove.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "src/client.h"
#include "src/filelist.h"
#include "src/read.h"
#include "src/util/directory_exists.h"
#include "src/util/get_server_manifest.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"

void upgrade_client(char *project_name)
{
    /* check if local version of project exists */
    //FAILURE CASES
    //the project doesn't exist
    if (!directory_exists(project_name))
    {
        printf("upgrade_client: [upgrade] Project does not exist locally. You need to create the project, first.\n");
        return;
    }
    //the client can't connect

    //1. Fail if there is a .Conflict file -- client tells user to resolve conflicts and update again
    char *conflict_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(conflict_path, "projects/%s/.Conflict", project_name);
    if(file_exists(conflict_path)){
        printf("Error: Can not upgrade, there is a conflict. First resolve all the conflicts, and then update.\n");
        return;
    }

    char *update_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(update_path, "projects/%s/.Update", project_name);
    FileList *update_files = NULL; 
  
    //2. Fail if the .Update does not exist 
    //in the case that the .Update file does exist on the server, READ .Update file.
    if(!file_exists(update_path)) {
        printf("Error: first run the update.\n");
        return;
    } else {
        printf("upgrade_client: Start reading client .Update\n");
        update_files = read_update_file(project_name);
        printf("upgrade_client: Done reading client .Update\n");
    }
    //if the .Update file is empty, print "up to date" and tell the server that
    if(update_files == NULL){
        printf("Up to date (nothing in .Update file)\n");
        remove(update_path);
        return;
    }

    printf("Procees .Update 'D' file:\n");
    FileList* update_item = update_files;
    while (update_item != NULL) {
        if (update_item->file_status == 'D') {
            printf("%c %s %s\n", update_item->file_status, update_item->file_path, update_item->file_hash);
            // Remove file from client's .Manifest.
            remove_client(project_name, update_item->file_path);
        } 
    }
    
    // Get client .manifest .......................................................
    printf("upgrade_client: Start reading client manifest\n");
    char *manifest_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(manifest_path, "projects/%s/.Manifest", project_name);
    Manifest *local_manifest = manifest_read(project_name);
    FileList* local_files = local_manifest->filelist;
    printf("upgrade_client: Done reading client manifest\n");

    //Get server manifest ..........................................................
    printf("Getting server side manifest\n");
    Manifest* server_manifest = get_server_manifest(project_name);
    FileList* server_files = server_manifest->filelist;

    printf("Procees .Update 'M' and 'A' file:\n");
    update_item = update_files;
    while (update_item != NULL) {
        
        if (update_item->file_status == 'M') {
            printf("%c %s %s\n", update_item->file_status, update_item->file_path, update_item->file_hash);
            // Have the client file overwritten by the one from the server and have its hash and version udated in the client .Manifest.
            // Write server item file to client side.
            char *item_file_path = calloc(strlen(project_name) + 50, sizeof(char));
            sprintf(item_file_path, "projects/%s/%s", project_name, update_item->file_path);
            printf("upgrade_client: Writing server file to %s", item_file_path);
            int item_file_fd = creat(item_file_path, 0777);
            dprintf(item_file_fd, "%s", update_item->file_bytes);
            close(item_file_fd);

            // Update local_manifest item hash and version  from server_manifest.
            // Search update_item in local_manifest
            FileList* local_manifest_item  = get_file_from(local_manifest->filelist, update_item->file_path);
            if(local_manifest_item == NULL) {
                printf("upgrade_client Error: client .Update file %s was not found in client .Manifest\n", update_item->file_path);
                return;
            }
            // Search update_item in server_manifest
            FileList* server_manifest_item = get_file_from(server_manifest->filelist, update_item->file_path);
            if(server_manifest_item == NULL) {
                printf("upgrade_client Error: client .Update file %s was not found in server .Manifest\n", update_item->file_path);
                return;
            }
            local_manifest_item->file_hash    = server_manifest_item->file_hash;
            local_manifest_item->file_version = server_manifest_item->file_version;

        } else if (update_item->file_status == 'A') {
            // Add the file to client .Manifest and and copy the file from server to the client side.
            printf("%c %s %s\n", update_item->file_status, update_item->file_path, update_item->file_hash);
            // Have the client file overwritten by the one from the server and have its hash and version udated in the client .Manifest.
            // Write server item file to client side.
            char *item_file_path = calloc(strlen(project_name) + 50, sizeof(char));
            sprintf(item_file_path, "projects/%s/%s", project_name, update_item->file_path);
            printf("upgrade_client: Writing server file to %s", item_file_path);
            int item_file_fd = creat(item_file_path, 0777);
            dprintf(item_file_fd, "%s", update_item->file_bytes);
            close(item_file_fd);

            // Update local_manifest item hash and version  from server_manifest.
            // Search update_item in server_manifest
            FileList* server_manifest_item = get_file_from(server_manifest->filelist, update_item->file_path);
            if(server_manifest_item == NULL) {
                printf("upgrade_client Error: client .Update file %s was not found in server .Manifest\n", update_item->file_path);
                return;
            }
           
            // Add server_manifest_item to local_manifest
            //FileList* local_manifest_item = get_file_from(local_manifest->filelist, update_item->file_path);
            FileList* filelist_append(FileList* filelist, FileList* item);
            filelist_append(local_manifest->filelist, server_manifest_item);
        }

        update_item = update_item->next;
    }
    remove(update_path);
    manifest_write(project_name, local_manifest);
}

Response *upgrade_server(Request *request)
{
    Response* junk = NULL;
    return junk;
}

FileList *read_update_file(char *project_path)
{   

    // Reads .Update from a project path
    // Open .Update
    char *update_path = calloc(strlen(project_path) + 50, sizeof(char));
    sprintf(update_path, "%s/.Update", project_path);
    //fails if .update doesn't exist
    if(!file_exists(update_path)){
        return NULL;
    }
    int update_fd = open(update_path, O_RDONLY, 0777);

    // Turn .Update file into `FileList`
    printf("read_update_file begin\n");
    FileList* filelist = NULL; 
    char *line;
    while (strcmp(line = read_line(update_fd), "\0") != 0)
    {
        FileList *item = filelist_new();
        item->file_path = calloc(strlen(line), sizeof(char));
        item->file_hash = calloc(strlen(line), sizeof(char));

        // Scan in <file_status> <file_path> <file_hash> from line
        sscanf(line, "%c %s %s", &item->file_status, item->file_path, item->file_hash);
        printf("%c %s %s\n", item->file_status, item->file_path, item->file_hash);
        // Append item to filelist
        filelist = filelist_append(filelist, item);
    }
    close(update_fd);
    printf("read_update_file end\n");
    return filelist;
}
