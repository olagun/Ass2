#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {
  char* token;
  struct Token* next;
} Token;

Token* Token_new(char* token);
void Token_print(Token* token);
Token* Token_append(Token* head, Token* token);

#endif