#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Token* Token_new(char* token) {
  Token* new = malloc(sizeof(Token));
  new->token = strdup(token);
  new->next = NULL;
  return new;
}

void Token_print(Token* token) {
  if (token == NULL) return;
  printf("%s", token->token);
  Token_print(token->next);
}

Token* Token_append(Token* head, Token* token) {
  if (head == NULL) return token;
  head->next = Token_append(head->next, token);
  return head;
}