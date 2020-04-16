//Fails for 2 reasons - if the project name already exists on  the server or 
//the client can not communicate with the server. Otherwise, 
//the server will create a project folder with the given name, initialize a 
//.Manifest for it and send it to the client. The client will set up a local
//version of the project folder in its current directory and should place the 
//.Manifest the server sent in it.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "src/color.h"
#include "src/client.h"

void Create_client(char* project_name){
    char message[1000];
    sprintf(message, "create:%s:", project_name);
    char* response = Client_send(message);
}

char* Create_server(char* body) {
    char* project_name = strtok(strdup(body), ":");
    char* project_path = malloc(strlen("projects/") + strlen(project_name) + 1);
    strcpy(project_path, "projects/");
    strcat(project_path, project_name);

    if (mkdir(project_path, 0777) < 0)  {
        // note: will fail if project name already exists
        return "failure:";
    }

    if (creat(".Manifest", 0777) < 0) {
        return "failure:";
    }
    
    char message[1000];
    sprintf(message, GRN "Success!" RESET " created project '" BLU "%s" RESET "'", project_name);

    // return serialize(project_name)
    return "created project";
}
