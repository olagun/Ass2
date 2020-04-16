// will fail if the project does not exist on the client
//add an entry for the the file to its own .Manifest with a new version 
//number and hashcode.

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include "../../src/boolean/boolean.h"
#include "../../src/client/client.h"

// https://rosettacode.org/wiki/SHA-256#C

int main(int argc, char *argv[] ){
    add(NULL, fileAdd.txt);
    printf("%s", "add worked");
}

int add(char* project, char* file) {

    //if (stat(project, &st) > 0) {  //project exists on client
        int fd = open(".Manifest", O_WRONLY | O_CREAT | O_APPEND, 0600);
        int version = 0;
        versionLen = strlen(version);
        //find the size of the file
        struct stat s;
        stat(file, &s);
        size = s.s_size;

        unsigned char c[MD5_DIGEST_LENGTH];
        int bytes = 0;
        MD5_CTX mdContext;
        MD5_Init (&mdContext);
        while ((bytes = fread (data, 1, 1024, file)) != 0)
            MD5_Update (&mdContext, data, bytes);
        
        MD5_Final (c,&mdContext);
        //write to .Manifest file
        write(fd, version, versionLen);
        write(fd, MD5, strlen(MD5));
    // l}
}
