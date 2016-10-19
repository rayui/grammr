#ifndef LEXER
#define LEXER

#define MAXTOKSZ 24
#define MAXNAMESZ 24
#define MAXDIRECTIONSZ 6
#define MAXCOMMANDSIZE 256

enum TokenType {
  TOK_VERB,
  TOK_QUIT,
  TOK_CONJUNCTION,
  TOK_COMPLEX,
  TOK_PRONOUN,
  TOK_PREPOSITION,
  TOK_WORD,
  TOK_NUMBER,
  TOK_EOL,
  TOK_UNKNOWN
};

typedef struct Token {
  int i;
  enum TokenType type;
  char val[MAXTOKSZ];
  struct Token* next;
} Token;

#endif

char isNumber(char* val);
char isConjunction(char* val);
char isVerb(char* val);
char isConjunction(char* val);
char isVerbPhrasal(char* val);
enum TokenType tokenTypeFromValue(char* val);
Token* readtok(Token** tokens, char* input);
char getVerbTransitivityByName(char* name);
void lex(Token** tokens, char* input);
void free_tokens(Token** tokens);
