#ifndef LEXER
#define LEXER

enum TokenType {
  TOK_VERB,
  TOK_QUIT,
  TOK_CONJUNCTION,
  TOK_COMPLEX,
  TOK_PRONOUN,
  TOK_ARTICLE,
  TOK_PREPOSITION,
  TOK_WORD,
  TOK_NUMBER,
  TOK_EOL,
  TOK_UNKNOWN
};

typedef struct Token {
  int i;
  enum TokenType type;
  char val[128];
  struct Token* next;
} Token;

#endif

char isNumber(char* val);
char isVerb(char* val);
char isConjunction(char* val);
enum TokenType tokenTypeFromValue(char* val);
Token* readtok(Token** tokens, char* input);
char getVerbTransitivityByName(char* name);
void lex(Token** tokens, char* input);
void free_tokens(Token** tokens);
