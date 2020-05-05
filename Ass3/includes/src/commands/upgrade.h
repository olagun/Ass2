#ifndef UPGRADE_H
#define UPGRADE_H

#include "src/response.h"
#include "src/request.h"
#include "src/filelist.h"

void upgrade_client(char* project_name);
Response* upgrade_server(Request* request);
FileList* read_update_file(char* project_path);
#endif