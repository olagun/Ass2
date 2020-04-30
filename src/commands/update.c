#include "src/commands/commands.h"
#include "src/commands/update.h"
#include "src/commands/commit.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "src/client.h"
#include "src/filelist.h"
#include "src/util/directory_exists.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
#include "src/util/get_file_size.h"

// Status codes moved to commands.h
//const int get_manifest = 0;
//const int send_update = 2;

void update_client(char *project_name)
{
    /* check if local version of project exists */
    if (!directory_exists(project_name))
    {
        printf("update_client: [update] Project does not exist locally. You need to create the project, first.\n");
        return;
    }

    //create a .Update, which holds the changes from server to client
    char *update_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(update_path, "projects/%s/.Update", project_name);
    int update_fd = creat(update_path, 0777);

    char *conflict_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(conflict_path, "projects/%s/.Conflict", project_name);
    int conflict_fd = creat(conflict_path, 0777);

    if ((file_exists(update_path) && get_file_size(update_path) > 0) || file_exists(conflict_path))
    {
        printf("update_client: Contains .Update or .Conflict file. Could not be commit\n");
        return;
    }

    // Get client .manifest .......................................................
    printf("update_client: Start reading client manifest\n");
    char *manifest_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(manifest_path, "projects/%s/.Manifest", project_name);
    Manifest *local_manifest = manifest_read(project_name);
    printf("update_client: Done reading client manifest\n");

    // Get server .manifest .......................................................
    printf("Getting server side manifest\n");
    Request *request = request_new();
    request->command_name = "update";
    request->project_name = project_name;
    request->status_code = get_manifest;
    Response *response = client_send(request); // Request from the client to the server. Is it correct?

    if (response->status_code < 0)
    {
        printf("%s\n", response->message);
        return;
    }
    response_log(response);
    printf("update_client: Done reading server side manifest\n");

    // Full success case: compare the server .Manifest with the client .Manifest .......................
    if (response->project_version == local_manifest->project_version){ 
        // Write blank .Update file.
        dprintf(update_fd, "");
        close(update_fd);
        // Delete .Conflict file.
        remove(conflict_path);
        printf("Up To Date\n");
        return;
    }

    FileList *local_files = local_manifest->filelist; // client manifest
    FileList *server_files = response->filelist;      // server manifest

    // We are here if manifest versions don't match .....................................................
    // Failure case -- server has updated data for the client, but the user has changed that file locally.
    int num_conflicts = 0;

    // begin server-file while-loop
    while (server_files != NULL)
    {
        bool client_live_hash_matches = 0;
        bool client_server_hash_matches = 0;
        bool client_server_version_matches = 0;
        char *server_hash = server_files->file_hash;

        // If the file exists in the client's MANIFEST (local), then compare the hash and the version.
        local_files = local_manifest->filelist;
        char *client_hash = NULL;
        char *live_hash = NULL;
        // Serch file in client's Manifest:
        FileList *client_file = get_file_from(local_files, server_files->file_path);
        if (client_file != NULL)
        {
            client_hash = client_file->file_hash;
            if ((strcmp(server_hash, client_hash) == 0))
            {
                client_server_hash_matches = 1;
            }
            if (client_file->file_version == server_files->file_version)
            {
                client_server_version_matches = 1;
            }
        }
        else
        {
            client_server_hash_matches = 1;
            client_server_version_matches = 1;
        }

        // If the file exists in the local DIRECTORY, then compare the live hash.
        if (file_exists(server_files->file_path))
        { // real file
            live_hash = get_file_hash(project_name, server_files->file_path);
            if (strcmp(client_hash, live_hash) == 0)
            {
                client_live_hash_matches = 1;
            }
        }
        else
        {
            client_live_hash_matches = 1;
        }

        // FAILURE CASE: if server and client.Mainfest hash are different, then it is a conflict.
        if (!(client_server_hash_matches && client_live_hash_matches))
        {
            // Write to Conflict file.
            num_conflicts++;
            dprintf(conflict_fd, "C %s %s\n", server_files->file_path, server_files->file_hash);
            printf("C %s %s\n", server_files->file_path, server_files->file_hash);
        }

        // THREE PARTIAL SUCCESS CASES ---------------------------------------------------------------
        // CASE 1. Server has file modifications for the client.
        if (!client_server_hash_matches && !client_server_version_matches)
        {
            if (client_live_hash_matches)
            {
                dprintf(update_fd, "M %s %s\n", server_files->file_path, server_files->file_hash);
                printf("M %s\n", server_files->file_path);
            }
        }

        // CASE 2. Server has files that were added to the project.
        if (client_file == NULL)
        {
            // client's Manifest does not have a file that appears in the server.
            dprintf(update_fd, "A %s %s\n", server_files->file_path, server_files->file_hash);
            printf("M %s\n", server_files->file_path);
        }

        server_files = server_files->next;
    } // end server-file while-loop
    printf("[update] num_conflicts %d\n", num_conflicts);

    // PARTIAL SUCCESS CASE 3. Server has removed files from the project.
    local_files = local_manifest->filelist; // client manifest
    server_files = response->filelist;      // server manifest

    // begin client-file while-loop
    while (local_files != NULL){
        // Serch the client-file in server's Manifest:
        FileList *server_file = get_file_from(server_files, local_files->file_path);
        if (server_file == NULL){
            // client's Manifest DOES have a file that DOES NOT appears in the server.
            dprintf(update_fd, "D %s %s\n", server_files->file_path, server_files->file_hash);
            printf("D %s\n", server_files->file_path);
        }
        local_files = local_files->next;
    } // end client-file while-loop

    //file doesn't exist locally, write to .Update file
    close(update_fd);

    //if conflict exists, delete .update file, if no conflicts exist delete .conflict file.
    if (num_conflicts > 0){
        if(file_exists(update_path)){
            remove(update_path);
        }
    } else {
        if(file_exists(conflict_path)){
            remove(conflict_path);
        }
    }
}

Response *update_server(Request *request)
{
    // Called by server side. Accepts request from client.
    if (request->status_code == get_manifest)
    {
        char *full_path = calloc(strlen(request->project_name) + 50, sizeof(char));
        sprintf(full_path, "projects/%s", request->project_name);

        if (!directory_exists(full_path))
        {
            Response *response = response_new();
            response->status_code = -1;
            response->message = "Project doesn't exist on the server";
            return response;
        }
    }

    if (request->status_code == send_update)
    {
        FileList *update_filelist = request->filelist;
        filelist_write("updates", update_filelist);
    }
    return NULL;
}
