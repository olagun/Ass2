//The destroy command will fail if the project name doesn’t exist on the server 
//or the client can not communicate with it. On receiving a destroy command the 
//server should lock the repository, expire any pending commits, delete all files
// and subdirectories under the project and send back a success message.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>

#include "../../src/client/client.h"

// char* destroy(char* project) {
    
    // char* directory = "direct";
    // //fails if name doesn't exist or client can't communicate
    // struct stat st = {0};
    // if (stat(directory, &st) == -1) {  //replace some directory with actual pathname
    //     if(Server_create(8000) == NULL){ //how to access actualt port #
    //         printf("%s", "destroy command failed");
    //     }
    // }

    //lock repository, expire commits, delete all the files
    //receives a name of from WTFClient, access the thread of that project
    //a thread is made for every new connection, mutex only for project (locking the mutex for the project)
    //client send

    //make a server folder and client folder to test

// }