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

// int update(char * proj_name)
// {
//     /* check if local version of project exists */
//     if (!directory_exists(proj_name))
//     {
//         printf("[update] Project does not exist locally.\n");
//         return -1;
//     }
       
//     char* update_path = calloc(strlen(project_name) + 50, sizeof(char));
//     sprintf(update_path, "projects/%s/.Update", project_name);

//     char* conflict_path = calloc(strlen(project_name) + 50, sizeof(char));
//     sprintf(conflict_path, "projects/%s/.Conflict", project_name);
// }
