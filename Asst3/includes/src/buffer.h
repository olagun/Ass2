#include <zlib.h>

#ifndef BUFFER_H
#define BUFFER_H

typedef struct Buffer {
  unsigned char* buffer;
  uLong offset;
  uLong length;
} Buffer;

Buffer* buffer_new(unsigned char* buffer, uLong length);
char* buffer_read_until(Buffer* buffer, char delim);
unsigned char* buffer_read_nbytes(Buffer* buffer, int n);

#endif