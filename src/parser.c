#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/main.h"
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/instruction.h"
#include "../include/interpreter.h"
#include "../include/lexer.h"
#include "../include/parser.h"

static int parser_counter = 0;
static char parser_word_reg[DEFAULTSTRINGSIZE];
static char parser_action_reg[DEFAULTSTRINGSIZE];
static NameList* parser_name_stack = NULL;
static NameList* parser_name_stack_tail = NULL;
static char* subject = NULL;
Token* currToken;

extern Location* currentLocation;
extern Item* items;
extern Actions* actions;
extern enum RunState RUNSTATE;
extern ItemList* inventory;

void parser_readtok() {
  currToken = currToken->next;
  parser_counter++;
}

void parser_set_action_reg() {
  memset(parser_action_reg, 0, DEFAULTSTRINGSIZE);
  strcpy(parser_action_reg, currToken->val);
}

void parser_concat_action_reg(char* split) {
  strcat(parser_action_reg, split);
  strcat(parser_action_reg, currToken->val);
}

void parser_set_word_reg() {
  if (strlen(parser_word_reg)) {
    strcat(parser_word_reg, " ");
  }
  strcat(parser_word_reg, currToken->val);
}

void parser_empty_word_reg() {
  memset(parser_word_reg, 0, DEFAULTSTRINGSIZE);
}

void parser_empty_name_list() {
  SGLIB_LIST_MAP_ON_ELEMENTS(NameList, parser_name_stack, mappedName, next, {
    SGLIB_LIST_DELETE(NameList, parser_name_stack, mappedName, next);
    free(mappedName);
  });
  parser_name_stack_tail = NULL;
  parser_name_stack = NULL;
}

void parser_add_word_reg_to_name_list() {
  struct NameList* newName = malloc(sizeof(struct NameList));
  strcpy(newName->name, parser_word_reg);

  SGLIB_LIST_ADD_AFTER(struct NameList, parser_name_stack_tail, newName, next);
  parser_name_stack_tail = newName;

  if (parser_name_stack == NULL) {
    parser_name_stack = parser_name_stack_tail;
  }

  parser_empty_word_reg();
}

int parser_peek(enum TokenType type) {
  if (currToken->type == type) {
    return 1;
  }
  return 0;
}

int parser_peekVal(char *s) {
  return (strcmp(currToken->val, s) == 0);
}

int parser_accept(enum TokenType type) {
  if (parser_peek(type)) {
    switch (type) {
      case TOK_WORD:
        parser_set_word_reg();
        break;
      case TOK_VERB:
        parser_set_action_reg();
        break;
      case TOK_PREPOSITION:
      case TOK_CONJUNCTION:
        parser_concat_action_reg(" ");
        break;
    }
    parser_readtok();
    return 1;
  }
  return 0;
}

void parser_error(enum ErrorType error, char* val) {
  create_error(SE_WARN, error, val);
  parser_readtok();
}

void parser_item() {
  if (parser_accept(TOK_WORD)) {
    while (parser_accept(TOK_WORD)) {}
    parser_add_word_reg_to_name_list();
  } else {
    parser_error(ERR_ITEM_EXPECTED, currToken->val);
  }
}

char* parser_action_object_clause() {
  if (parser_accept(TOK_CONJUNCTION)) {
    parser_accept(TOK_ARTICLE);
    parser_item();
    if (parser_name_stack_tail != NULL) {
      return parser_name_stack_tail->name;
    }
  }
  return NULL;
}

void parser_action(char* output) {
  Token* token = currToken;
  Actions* action = NULL;
  Item* item = NULL;
  char* object = NULL;
  char numArgs = 0;

  parser_accept(TOK_VERB);
  parser_accept(TOK_PREPOSITION);

  if (parser_peek(TOK_WORD) || parser_accept(TOK_ARTICLE)) {
    parser_item();
    if (parser_name_stack != NULL) {
      if (subject != NULL) {
        free(subject);
      }
      subject = malloc(strlen(parser_name_stack->name) + 1);
      strcpy(subject, parser_name_stack->name);
      object = parser_action_object_clause();
    }
  } else if (parser_accept(TOK_PRONOUN)) {
    if (subject == NULL) {
      parser_error(ERR_ITEM_NOT_FOUND, "it");
      return;
    }
    object = parser_action_object_clause();
  } else {
    if (subject != NULL) {
      free(subject);
      subject = NULL;
    }
  }

  if (subject) {
    if (intrpt_in_context(subject)) {
      numArgs = 1;
      if (object) {
        if (intrpt_in_context(object)) {
          numArgs = 2;
        } else {
          parser_error(ERR_ITEM_NOT_FOUND, object);
          return;
        }
      }
    } else {
      parser_error(ERR_ITEM_NOT_FOUND, subject);
      return;
    }
  }

  if (numArgs > 0) {
    item = findItemByName(subject);
    action = findItemAction(actions, item->actions, parser_action_reg, numArgs);
  }
     
  if (action == NULL) {
    action = findDefaultAction(actions, parser_action_reg, numArgs);
    if (action == NULL) {
      parser_error(ERR_NO_SUCH_ACTION, parser_action_reg);
      return;
    }
  }
  
  interpret(subject, object, action->instructions, output);
}

void parser_command(char* output) {
  parser_empty_name_list();

  if (parser_peek(TOK_VERB)) {
    parser_action(output);
  } else if (parser_accept(TOK_QUIT)) {
    parser_quit();
  } else {
    parser_error(ERR_COMMAND_EXPECTED, currToken->val);
  }
}

void parser_quit() {
  create_error(SE_TERMINAL, ERR_QUIT, currToken->val);
}

void parse(Token** tokenHead, char* output) {
  currToken = *tokenHead;
  parser_counter = 0;

  while(currToken != NULL && RUNSTATE == SE_OK) {
    parser_command(output);

    if (parser_accept(TOK_EOL) || parser_accept(TOK_COMPLEX)) {
      //noop
    } else {
      parser_error(ERR_END_OF_COMMAND_EXPECTED, currToken->val);
    }
  }
}