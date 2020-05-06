#include "src/buffer.h"

#include <stdlib.h>
#include <zlib.h>

Buffer* buffer_new(unsigned char* buffer, uLong length) {
  Buffer* new = calloc(1, sizeof(Buffer));
  new->buffer = buffer;
  new->length = length;
  new->offset = 0;
  return new;
}

char* buffer_read_until(Buffer* buffer, char delim) {
  int start_offset = buffer->offset;

  while (buffer->offset < buffer->length) {
    if (delim == buffer->buffer[buffer->offset]) {
      buffer->buffer[buffer->offset] = '\0';
      buffer->offset++;
      break;
    }

    buffer->offset++;
  }

  return (char*)buffer->buffer + start_offset;
}

unsigned char* buffer_read_nbytes(Buffer* buffer, int n) {
  int start_offset = buffer->offset;
  buffer->offset += n;
  return buffer->buffer + start_offset;
}