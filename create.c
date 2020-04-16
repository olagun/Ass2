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

#include "src/client.h"

int main(int argc, char** argv) {
    
    char* directory = "direct";
    //creates the project folder
    struct stat st = {0};
    if (stat(directory, &st) == -1) {  //replace some directory with actual pathname
        if(server_create(8000) == NULL){ //how to access actualt port #
            mkdir("/some/directory", 0700);
        }
    }

    //create the .Manifest file
    char *filepath = malloc(strlen(directory) + strlen(".Manifest") + 2);
    filepath = strcpy(filepath, directory);
    filepath = strcat(filepath, ".Manifest");
    
    int filedescriptor = open(*filepath, O_RDWR |O_CREAT);
    if (filedescriptor < 0){
        perror("Error creating .Manifest file\n");
        exit(-1);
    }
}

