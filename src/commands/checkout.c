#include "src/commands/checkout.h"
#include "src/manifest.h"
#include "src/commands/commands.h"


#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "src/client.h"
#include "src/util/directory_exists.h"
#include "src/util/get_server_manifest.h"
#include "src/util/file_exists.h"

void checkout_client(char* project_name) {
   //FAILURE CASES

    //1. Fail if project already exists locally
    if(directory_exists(project_name)){
        printf("[checkout] Project already exists locally.\n");
        return;
    }
  
    //2. Fail if the .configure does not exist 
    char *configure_path = calloc(strlen(project_name) + 50, sizeof(char));
    sprintf(configure_path, ".configure");
    //FileList *configure_files = NULL; 
    if(!file_exists(configure_path)) {
        printf("Error: first run configure.\n");
        return;
    } 

    //3. Fail if the project doesn't exist on the server
    //response == NULL
    Manifest* server_manifest = get_server_manifest(project_name);
    if(server_manifest == NULL){
        printf("Error: project does not exist on the server.\n");
        return;
    }

    // 4. Manifest exists on the server, so write the project directory structure to client side.
    int ret_status = mkdir(project_name, 0777);
    if(ret_status != 0){
        printf("Error: unable to create project directory %s.\n", project_name);
    }
    filelist_write(project_name, server_manifest->filelist);

    // 5. Save manifest to client side.
     manifest_write(project_name, server_manifest);
}

// void checkout_server(Request *request){
//     // Called by server side. Accepts request from client.
//     if (request->status_code == get_manifest)
//     {
//         char *full_path = calloc(strlen(request->project_name) + 50, sizeof(char));
//         sprintf(full_path, "projects/%s", request->project_name);

//         if (!directory_exists(full_path))
//         {
//             Response *response = response_new();
//             response->status_code = -1;
//             response->message = "Project doesn't exist on the server";
//             return response;
//         }
//     }

//     if (request->status_code == send_update)
//     {
//         FileList *update_filelist = request->filelist;
//         filelist_write("updates", update_filelist);
//     }
//     return NULL;
// }