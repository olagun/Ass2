#include "src/filelist.h"

#ifndef COMPRESSION_H
#define COMPRESSION_H

void write_filelist_compressed(int fd, FileList* list);
FileList* read_compressed_filelist(int fd);

#endif