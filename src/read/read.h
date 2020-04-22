#ifndef READ_H
#define READ_H

char* read_until(int fd, char delim);
char* read_nbytes(int fd, int n);
char* read_line(int fd);

#endif