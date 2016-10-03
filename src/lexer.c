#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/lexer.h"

const char SPLIT_CHAR[2] = " \0";
int counter;
extern Verbs* verbs;
extern TokenList* tokenList;
extern ErrorList* errorList;
char ERR;

TokenList* lastToken;

char isNumber(char* val) {
  return isStringNumeric(val);
}

char isConjunction(char* val) {
  if (toLowerCaseCompare(val, "with") || toLowerCaseCompare(val, "and")) {
    return 1;
  }
  return 0;
}

char isPreposition(char* val) {
  if (toLowerCaseCompare(val, "in") || toLowerCaseCompare(val, "out") ||
      toLowerCaseCompare(val, "on") || toLowerCaseCompare(val, "off") ||
      toLowerCaseCompare(val, "for") || toLowerCaseCompare(val, "to")
  ) {

    return 1;
  }
  return 0;
}

char isPronoun(char* val) {
  if (toLowerCaseCompare(val, "the")) {
    return 1;
  }
  return 0;
}

char isComplex(char* val) {
  if (toLowerCaseCompare(val, "then")) {
    return 1;
  }
  return 0;
}

char isVerb(char* val) {
  char ret = 0;

  SGLIB_LIST_MAP_ON_ELEMENTS(Verbs, verbs, verb, next, {
    if (toLowerCaseCompare(verb->name, val)) {
      ret = 1;
    }
  });

  return ret;
}

Verbs* getVerbByName(char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Verbs, verbs, verb, next, {
    if (toLowerCaseCompare(name, verb->name)) {
      return verb;
    }
  });

  return NULL;
}

char getVerbTransitivityByName(char* name) {
  Verbs* verb = getVerbByName(name);
  if (verb != NULL) {
    return verb->transitivity;
  }
  return NULL;
}

enum TokenType tokenTypeFromValue(char* val) {
  //first look for conjunctions, token type is TOK_CONJUNCTION
  //then , if token appears somewhere in list of actions, token type is TOK_VERB
  if (isConjunction(val)) {
    return TOK_CONJUNCTION;
  } else if (isPronoun(val)) {
    return TOK_PRONOUN;
  } else if (isPreposition(val)) {
    return TOK_PREPOSITION;
  } else if (isComplex(val)) {
    return TOK_COMPLEX;
  } else if (isVerb(val)) {
    return TOK_VERB;
  } else if (isNumber(val)) {
    ERR = SE_WARNING;
    create_error(LEX_UNRECOGNISED_TOKEN, val);
    return TOK_NUMBER;
  } else if (isStringAlpha(val)) {
    return TOK_WORD;
  } else {
    return TOK_EOL;
  }

  return NULL;
}

enum TokenType readtok(char* input) {
  char *val;
  TokenList* token = malloc(sizeof(TokenList));

  if (input != NULL) {
    val = strtok(input, SPLIT_CHAR);
  } else {
    val = strtok(NULL, SPLIT_CHAR);
  }

  token->type = tokenTypeFromValue(val);
  strcpy(token->val, val);

  SGLIB_LIST_ADD(TokenList, tokenList, token, next);
  lastToken = token;

  counter++;

  return token->type;
}

struct Verbs* createVerb(char* name, char transitivity) {
  Verbs* verb = malloc(sizeof(Verbs));

  if (verb != NULL) {
    strcpy(verb->name, name);
    verb->transitivity = transitivity;
  }

  return verb;
}

int lex(char* source) {
  enum TokenType type;
  static char input[MAXCOMMANDSIZE];

  ERR = SE_OK;
  counter = 0;

  strncpy(input, source, MAXCOMMANDSIZE - 2);
  input[MAXCOMMANDSIZE - 1] = '\0';

  type = readtok(input);
  while(type != TOK_EOL) {
    type = readtok(NULL);
  }

  SGLIB_LIST_REVERSE(struct TokenList, tokenList, next);

  return ERR;
}

void free_lexer() {
  SGLIB_LIST_MAP_ON_ELEMENTS(struct TokenList, tokenList, token, next, {
    SGLIB_LIST_DELETE(struct TokenList, tokenList, token, next);
    free(token);
  });
}
