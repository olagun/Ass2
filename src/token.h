#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {
  char* token;
  struct Token* next;
} Token;

Token* Token_new(char* token);
int Token_size(Token* head);
char* Token_to_string(Token* head);
void Token_write(char* filename, Token* token);
void Token_print(Token* token);
Token* Token_append(Token* head, Token* token);
Token* Token_read(char* filename);

#endif