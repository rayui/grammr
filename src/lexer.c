#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/lexer.h"

const char SPLIT_CHAR[2] = " \0";
extern ErrorList* errorList;
char ERR;

char isNumber(char* val) {
  return isStringNumeric(val);
}

char isConjunction(char* val) {
  if (strComp(val, "with") || strComp(val, "and")) {
    return 1;
  }
  return 0;
}

char isPreposition(char* val) {
  if (strComp(val, "in") || strComp(val, "out") ||
      strComp(val, "on") || strComp(val, "off") ||
      strComp(val, "for") || strComp(val, "to") ||
      strComp(val, "at") || strComp(val, "with") ||
      strComp(val, "up") || strComp(val, "down")
  ) {
    return 1;
  }
  return 0;
}

char isPronoun(char* val) {
  if (strComp(val, "the")) {
    return 1;
  }
  return 0;
}

char isComplex(char* val) {
  if (strComp(val, "then")) {
    return 1;
  }
  return 0;
}

char isVerb(char* val) {
  if (strComp(val, "look") || strComp(val, "go") ||
      strComp(val, "use") || strComp(val, "put") ||
      strComp(val, "take") || strComp(val, "call")
  ) {
    return 1;
  }
  return 0;
}

enum TokenType tokenTypeFromValue(char* val) {
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

//get this to return the actual token rather than token type
//then we can use it to add to the next one
//so we don't have to reverse the list every time

Token* readtok(Token** tail, char* input) {
  char *val;
  Token* token = malloc(sizeof(Token));

  if (input != NULL) {
    val = strtok(input, SPLIT_CHAR);
  } else {
    val = strtok(NULL, SPLIT_CHAR);
  }

  toLowerCase(val);

  token->type = tokenTypeFromValue(val);

  if (token->type != TOK_EOL) {
    strcpy(token->val, val);
  } else {
    strcpy(token->val, "\0");
  }

  SGLIB_LIST_ADD_POST(Token, *tail, token, next);

  return token;
}

int lex(Token** tokenHead, char* source) {
  enum TokenType type;
  Token* tail = NULL;
  static char input[MAXCOMMANDSIZE];

  ERR = SE_OK;

  strncpy(input, source, MAXCOMMANDSIZE - 1);
  input[MAXCOMMANDSIZE] = '\0';

  tail = readtok(tokenHead, input);
  while(tail->type != TOK_EOL) {
    tail = readtok(&tail, NULL);
  }

  return ERR;
}

void free_tokens(Token** tokens) {
  SGLIB_LIST_MAP_ON_ELEMENTS(struct Token, *tokens, token, next, {
    SGLIB_LIST_DELETE(struct Token, *tokens, token, next);
    free(token);
  });
}
