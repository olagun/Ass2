#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/filelist.h"
#include "src/manifest.h"
#include "src/util/color.h"
#include "src/util/file_exists.h"
#include "src/util/get_file_hash.h"

void remove_client(char *project_name, char *file_path) {
  char full_path[1000] = {0};
  sprintf(full_path, "%s/%s", project_name, file_path);

  if (!file_exists(full_path)) {
    printf(BRED "[Remove Error]" RESET
                " Can't remove '%s' becuase it doesn't exist\n",
           full_path);
    return;
    
  }

  Manifest *manifest = manifest_read(project_name);
  FileList* maifest_file = get_file_from(manifest->filelist, file_path);
  if(maifest_file !=NULL ) {
    manifest->filelist = filelist_remove(manifest->filelist, file_path);
    manifest_write(project_name, manifest);
    printf(BWHT "[Remove]" RESET " Successfully removed file '%s'\n", file_path);
  } else {
      printf(BRED "[Remove Error]" RESET " File '%s' does not exist.\n", file_path);
  }
}
