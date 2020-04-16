#include "token.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Token* Token_new(char* token) {
  Token* new = malloc(sizeof(Token));
  new->token = strdup(token);
  new->next = NULL;
  return new;
}

int Token_size(Token* head) {
  if (head == NULL) return 0;
  return 1 + Token_size(head->next);
}

char* Token_to_string(Token* head) {
  int length = Token_size(head);
  char* string = malloc(sizeof(char) * length + 1);

  int index = 0;
  while (head != NULL) {
    string[index] = *head->token;
    head = head->next;
    index++;
  }
  string[index] = '\0';

  return string;
}

void Token_print(Token* token) {
  if (token == NULL) return;
  printf("%s", token->token);
  Token_print(token->next);
}

void Token_write(char* filename, Token* head) {
  if (head == NULL) return;
  int write_fd = creat(filename, 0777);

  while (head != NULL) {
    write(write_fd, head->token, strlen(head->token));
    head = head->next;
  }

  close(write_fd);
}

Token* Token_append(Token* head, Token* token) {
  if (head == NULL) return token;
  head->next = Token_append(head->next, token);
  return head;
}

// reads a file into a linked list
Token* Token_read(char* filename) {
  if (filename == NULL) return NULL;

  int fd = open(filename, O_RDONLY, 0777);

  Token* head = NULL;
  char* buffer = malloc(sizeof(char) * 2);
  while (read(fd, buffer, 1) > 0) {
    Token* token = Token_new(buffer);
    head = Token_append(head, token);
  }

  return head;
}