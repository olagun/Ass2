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

Token* token_append(Token* head, Token* token) {
  if (head == NULL) return token;
  head->next = token_append(head->next, token);
  return head;
}