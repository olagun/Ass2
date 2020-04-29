#ifndef UPDATE_H
#define UPDATE_H

#include "src/filelist.h"
#include "src/token.h"
#include "src/request.h"
#include "src/response.h"

void update_client(char *project_name);
FileList *get_file_from(FileList *file_list, char *in_file_path);
Response* update_server(Request* request);

#endif