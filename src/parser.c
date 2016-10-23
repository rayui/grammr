#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/instruction.h"
#include "../include/lexer.h"
#include "../include/parser.h"

int parser_counter = 0;
char parser_word_reg[MAXNAMESZ];
char parser_action_reg[MAXNAMESZ];
NameList* parser_name_stack = NULL;
NameList* parser_name_stack_tail = NULL;
Token* currToken;
InstructionList* lastInstruction;

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
  memset(parser_action_reg, 0, MAXNAMESZ);
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
  memset(parser_word_reg, 0, MAXNAMESZ);
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
  if (parser_name_stack == NULL) {
    parser_name_stack = parser_name_stack_tail;
  }
  parser_name_stack_tail = newName;

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
  create_error(SE_PARSER, error, val);
  parser_readtok();
}

void parser_item() {
  parser_accept(TOK_PRONOUN);
  if (parser_accept(TOK_WORD)) {
    while (parser_accept(TOK_WORD)) {}
    parser_add_word_reg_to_name_list();
  } else {
    parser_error(ERR_ITEM_EXPECTED, currToken->val);
  }
}

void parser_action(InstructionList** instructions) {
  Token* token = currToken;
  Actions* action = NULL;
  Item* item = NULL;
  char* subject = NULL;
  char* object = NULL;

  parser_accept(TOK_VERB);
  parser_accept(TOK_PREPOSITION);

  if (parser_peek(TOK_WORD) || parser_peek(TOK_PRONOUN)) {
    parser_item();
    if (parser_accept(TOK_CONJUNCTION)) {
      parser_item();
    }
  }

  subject = parser_name_stack ? parser_name_stack->name : NULL;

  if (subject) {
    object = parser_name_stack->next ? parser_name_stack->next->name : NULL;
    item = findItemByName(subject);
    if (item != NULL) {
      if (object) {
        strcat(parser_action_reg, " ");
        strcat(parser_action_reg, object);
      }
      action = findActionByNameAndItem(actions, item, parser_action_reg);
      if (action == NULL)
        action = findDefaultActionByName(actions, parser_action_reg);
      
    } else if (findLocationByName(subject)) {
      action = findDefaultActionByName(actions, parser_action_reg);
    } else {
      parser_error(ERR_ITEM_NOT_FOUND, subject);
    }
  } else {
    action = findDefaultActionByName(actions, parser_action_reg);
  }

  if (action == NULL) {
    parser_error(ERR_NO_SUCH_ACTION, parser_action_reg);
    return;
  }

  lastInstruction = inst_insert(instructions, action->instructions, lastInstruction, subject, object);  
}

void parser_command(InstructionList** instructions) {
  parser_empty_name_list();

  if (parser_peek(TOK_VERB)) {
    parser_action(instructions);
  } else if (parser_accept(TOK_QUIT)) {
    parser_quit();
  } else {
    parser_error(ERR_COMMAND_EXPECTED, currToken->val);
  }
}

void parser_commands(InstructionList** instructions) {
  parser_command(instructions);

  if (parser_accept(TOK_EOL)) {
    parser_eol();
  } else if (parser_accept(TOK_COMPLEX)) {
    parser_commands(instructions);
  } else {
    parser_error(ERR_END_OF_COMMAND_EXPECTED, currToken->val);
  }
}

void parser_quit() {
  parser_error(ERR_QUIT, currToken->val);
}

void parser_eol() {
  return;
}

void parse(Token** tokenHead, InstructionList** instructions) {
  currToken = *tokenHead;
  lastInstruction = *instructions;

  parser_counter = 0;

  while(currToken != NULL && RUNSTATE == SE_OK) {
    parser_commands(instructions);
  }
}