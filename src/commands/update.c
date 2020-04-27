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

int update(char * proj_name)
{
        /* check if local version of project exists */
        if (!directory_exists(proj_name))
        {
                fprintf(stderr, "[update] Project does not exist locally.\n");
                return 1;
        }
       /* check if .Update exists from previous iteration - if it does, direct user to run upgrade first */
        char dot_update_path[strlen(proj_name) + strlen("/.Update")];
        sprintf(dot_update_path, "%s/.Update", proj_name);
        if (file_exists(dot_update_path))
        {
                fprintf(stderr, "[update] An existing .Update file already exists. Please run upgrade before running update again.\n");
                return 1;
        }

        /* begin by initializing a connection to the server */
        int sd = create_client();
        if (sd == -1)
        {
                fprintf(stderr, "[_update] Error connecting to server.");
                return 1;
        }
        /* fetch manifest file from server and store in a linked list */
        char * manifest_contents = fetch_server_manifest(sd, proj_name);
        if (manifest_contents == NULL)
        {
                fprintf(stderr, "[_update] Error creating manifest. FILE %s. LINE: %d.\n", __FILE__, __LINE__);
                return 1;
        }
        // maybe declare a linked list for contents of manifest?: manifest_entry* server_manifest = manifest_read(manifest_contents);
        free(manifest_contents); /* free buffer of manifest file bc we don't need it anymore */
}