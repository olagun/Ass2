#include "token.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Token* token_new(char* token) {
  Token* new = calloc(1, sizeof(Token));
  new->token = strdup(token);
  return new;
}

int token_size(Token* head) {
  if (head == NULL) return 0;
  return 1 + token_size(head->next);
}

char* token_string(Token* head) {
  int length = token_size(head);
  char* string = calloc(length + 1, sizeof(char));

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

void token_write(char* filename, Token* head) {
  if (head == NULL) return;
  int write_fd = creat(filename, 0777);

  while (head != NULL) {
    write(write_fd, head->token, strlen(head->token));
    head = head->next;
  }

  close(write_fd);
}

Token* token_append(Token* head, Token* token) {
  if (head == NULL) return token;
  head->next = token_append(head->next, token);
  return head;
}

// Reads a file into a linked list
Token* token_read(char* filename) {
  if (filename == NULL) return NULL;

  int fd = open(filename, O_RDONLY, 0777);

  Token* head = NULL;
  char* buffer = malloc(sizeof(char) * 2);
  while (read(fd, buffer, 1) > 0) {
    Token* token = token_new(buffer);
    head = token_append(head, token);
  }

  return head;
}