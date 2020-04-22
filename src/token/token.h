#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {
  char* token;
  struct Token* next;
} Token;

Token* token_new(char* token);
char* token_string(Token* head);
int token_size(Token* head);
void token_write(char* filename, Token* token);
void token_print(Token* token);
Token* token_append(Token* head, Token* token);
Token* token_read(char* filename);

#endif