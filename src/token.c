#include "src/token.h"

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
  int total_length = 0;
  Token* curr = head;
  while (curr != NULL) {
    total_length += strlen(curr->token);
    curr = curr->next;
  }

  char* string = calloc(total_length + 1, sizeof(char));

  int bytes_written = 0;
  curr = head;
  while (curr != NULL) {
    int len = strlen(curr->token);
    memcpy(string + bytes_written, curr->token, len);
    bytes_written += len;
    curr = curr->next;
  }

  return string;
}

Token* token_append(Token* head, Token* token) {
  if (head == NULL) return token;
  head->next = token_append(head->next, token);
  return head;
}