#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/manifest.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"
// #include <openssl/md5.h>



int remove_client(char* project_name, char* file_path){
    //will remove the entry for the given file from its own .Manifest
    int full_path_len = strlen(file_path) + strlen(project_name);

    char pj[full_path_len];
    bzero(pj, full_path_len);
    sprintf(pj, "%s/.manifest", project_name);
    int length = strlen(file_path) + strlen(project_name) + 2;
    char name[length];
    bzero(name, length);
    sprintf(name, "%s/%s", project_name, file_path);

    int fd = open(pj, O_RDWR, 00600);

    //error in opening file
    if (fd == -1)
    {
        fprintf(stderr, "[_remove] Error opening %s. FILE: %s. LINE: %d.\n", pj, __FILE__, __LINE__);
        return 1;
    }

    //file doesn't exist
    if (!file_exists(file_path)) {
        printf("Remove Error: Can't remove a file that doesn't exist.");
        return;
    }

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char buf[size+1];
    if (better_read(fd, buf, size, __FILE__, __LINE__) != 1)
            return 1;
    close(fd);
    buf[size]='\0'; /* add string termination so that substring search doesn't look further than end of file */
    // printf("pj: %s\nbuf: %s\n", name, buf);
    char * line = strstr(buf, name);
    // printf("%s\n", line);
    if (line == NULL)
    {
        fprintf(stderr, "[_remove] (Ignore if adding...) %s not in manifest. FILE: %s. LINE: %d.\n", file_path, __FILE__, __LINE__);
        return 1;
    }
    int k = 0;
    while (line[k--] != '\n');
        k+=2;
    int num_bytes = &line[k] - &buf[0];
    int i = num_bytes;
    int end;
    while (buf[i] != '\n')
        i++;
    end = i+1;
        
    // printf("%d\n", num_bytes);
    int fd1 = open(pj, O_RDWR | O_TRUNC, 00600);
    if (fd1 == -1)
    {
        fprintf(stderr, "[_remove] Error opening %s. FILE: %s. LINE: %d.\n", pj, __FILE__, __LINE__);
        return 1;
    }
    //if (manifest_write(project_name, manifest) != 1)
     //   return 1;
        
    //if (manifest_write() != 1)
    //    return 1;

    close(fd1);
    printf("[_remove] File removed successfully.\n");
    return 0;
}

