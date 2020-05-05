#ifndef GET_SERVER_MANIFEST_H
#define GET_SERVER_MANIFEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "src/manifest.h"

Manifest* get_server_manifest(char* project_name);

#endif
