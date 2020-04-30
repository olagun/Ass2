

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/manifest.h"

#ifndef GET_SERVER_MANIFEST_H
#define GET_SERVER_MANIFEST_H

Manifest* get_server_manifest(char* project_name);

#endif
