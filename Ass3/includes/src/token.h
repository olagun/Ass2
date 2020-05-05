#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {
  char* token;
  struct Token* next;
} Token;

Token* token_new(char* token);
char* token_string(Token* head);
int token_size(Token* head);
Token* token_append(Token* head, Token* token);

#endif