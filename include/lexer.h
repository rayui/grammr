#ifndef LEXER
#define LEXER

#define MAXTOKSZ 24
#define MAXNAMESZ 24
#define MAXDIRECTIONSZ 6
#define MAXCOMMANDSIZE 512

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

typedef struct TokenList {
  enum TokenType type;
  char val[MAXTOKSZ];
  struct TokenList* next;
} TokenList;

typedef struct Verbs {
  char name[MAXTOKSZ];
  char transitivity;
  struct Verbs* next;
} Verbs;

#endif

char isNumber(char* val);
char isConjunction(char* val);
char isVerb(char* val);
char isConjunction(char* val);
char isVerbPhrasal(char* val);
enum TokenType tokenTypeFromValue(char* val);
enum TokenType readtok(char* input);
struct Verbs* createVerb(char* name, char transitivity);
char getVerbTransitivityByName(char* name);
int lex(char* input);
void free_lexer(void);
