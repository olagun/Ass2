#include "src/compression.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "src/buffer.h"
#include "src/filelist.h"
#include "src/read.h"
#include "src/util/color.h"

void write_filelist_compressed(int fd, FileList* list) {
  printf(BWHT "[Compression]" RESET " Compressing " UWHT "FileList" RESET
              "...\n");

  char tmp[1000];
  int file_count = filelist_size(list);

  /* Calculate the size of <file_count> followed by every `FileList` item */

  // <file_count> ":"
  uLong uncompressed_len = sprintf(tmp, "%d:", file_count);

  FileList* file;
  for (file = list; file != NULL; file = file->next) {
    // <file_path> ":"
    uncompressed_len += sprintf(tmp, "%s:", file->file_path);

    // <file_version> ":"
    uncompressed_len += sprintf(tmp, "%d:", file->file_version);

    // <file_hash> ":"
    uncompressed_len += sprintf(tmp, "%s:", file->file_hash);

    // <file_size> ":"
    uncompressed_len += sprintf(tmp, "%d:", file->file_size);

    // <file_bytes>
    uncompressed_len += file->file_size;
  }

  /* Copy <file_count> followed by every `FileList` item into memory */

  char* uncompressed_bytes = calloc(uncompressed_len, sizeof(char));

  // <file_count> ":"
  uLong offset = sprintf(uncompressed_bytes, "%d:", file_count);

  for (file = list; file != NULL; file = file->next) {
    // <file_path> ":"
    offset += sprintf(uncompressed_bytes + offset, "%s:", file->file_path);

    // <file version> ":"
    offset += sprintf(uncompressed_bytes + offset, "%d:", file->file_version);

    // <file_hash> ":"
    offset += sprintf(uncompressed_bytes + offset, "%s:", file->file_hash);

    // <file_size> ":"
    offset += sprintf(uncompressed_bytes + offset, "%d:", file->file_size);

    // <file_bytes>
    memcpy(uncompressed_bytes + offset, file->file_bytes, file->file_size);
    offset += file->file_size;
  }

  // Compressed bytes maye be larger than uncompressed bytes for small values
  // https://stackoverflow.com/questions/43985477/zlib-deflated-input-is-larger-than-original-input-string-of-chars
  uLong compressed_len = uncompressed_len + 1000;
  unsigned char* compressed_bytes = calloc(compressed_len, sizeof(char));

  int error_code =
      compress(compressed_bytes, &compressed_len,
               (unsigned char*)uncompressed_bytes, uncompressed_len);

  if (error_code != Z_OK) {
    printf(BRED "[Compression Error]" RESET " Could not compress " UWHT
                "FileList" RESET " because '%s'\n",
           zError(error_code));
    return;
  }

  float compression_ratio = (float)uncompressed_len / (float)compressed_len;
  printf(BWHT "[Compression]" RESET " Compressed " UWHT "FileList" RESET
              " with a compression ratio of %s%.2f" RESET "\n",
         compression_ratio > 1 ? BGRN : BRED, compression_ratio);

  // <uncompressed_len> ":" <compressed_len> ":" <compressed_bytes>
  dprintf(fd, "%lu:%lu:", uncompressed_len, compressed_len);
  write(fd, compressed_bytes, compressed_len);
}

// <uncompressed_len> ":" <compressed_len> ":" <compressed_bytes>
FileList* read_compressed_filelist(int fd) {
  printf(BWHT "[Compression]" RESET " Decompressing " UWHT "FileList" RESET
              "...\n");

  uLong expected_uncompressed_len = (uLong)atol(read_until(fd, ':'));
  uLong compressed_len = (uLong)atol(read_until(fd, ':'));

  unsigned char* compressed_bytes = read_nbytes(fd, compressed_len);

  unsigned char* uncompressed_bytes =
      calloc(expected_uncompressed_len, sizeof(char));

  uLong real_uncompressed_len = expected_uncompressed_len;
  int error_code = uncompress(uncompressed_bytes, &real_uncompressed_len,
                              compressed_bytes, compressed_len);

  if (error_code != Z_OK) {
    printf(BRED "[Compression Error]" RESET " Could not decompress " UWHT
                "FileList" RESET " because '%s'\n",
           zError(error_code));
    return NULL;
  }

  if (real_uncompressed_len != expected_uncompressed_len) {
    printf(BYEL "[Compression Warning]" RESET " Did not decompress " UWHT
                "FileList" RESET " as expected\n");
  }

  float compression_ratio =
      (float)real_uncompressed_len / (float)compressed_len;
  printf(BWHT "[Compression]" RESET " Decompressed " UWHT "FileList" RESET
              " with a compression ratio of %s%.2f" RESET "\n",
         compression_ratio > 1 ? BGRN : BRED, compression_ratio);

  Buffer* buffer = buffer_new(uncompressed_bytes, real_uncompressed_len);

  FileList* filelist = NULL;
  int file_count = atoi(buffer_read_until(buffer, ':'));

  for (; file_count > 0; file_count--) {
    FileList* file = filelist_new();

    file->file_path = buffer_read_until(buffer, ':');
    file->file_version = atoi(buffer_read_until(buffer, ':'));
    file->file_hash = buffer_read_until(buffer, ':');
    file->file_size = atoi(buffer_read_until(buffer, ':'));
    file->file_bytes = buffer_read_nbytes(buffer, file->file_size);

    filelist = filelist_append(filelist, file);
  }

  return filelist;
}
