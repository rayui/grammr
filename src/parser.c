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

enum RunState ERR = SE_OK;
int parser_counter = 0;
char parser_word_reg[MAXNAMESZ];
char parser_action_reg[MAXNAMESZ];
NameList* parser_name_stack;
Token* currToken;
InstructionList* lastInstruction;

extern Item* items;
extern Actions* actions;

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
}

void parser_add_word_reg_to_name_list() {
  struct NameList* newName = malloc(sizeof(struct NameList));
  strcpy(newName->name, parser_word_reg);

  SGLIB_LIST_ADD(struct NameList, parser_name_stack, newName, next);

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
  ERR = SE_WARNING;

  create_error(error, val);

  parser_readtok();
}

void parser_item() {
  parser_accept(TOK_PRONOUN);
  if (parser_accept(TOK_WORD)) {
    while (parser_accept(TOK_WORD)) {}
    parser_add_word_reg_to_name_list();
  } else {
    parser_error(CC_ITEM_EXPECTED, currToken->val);
  }
}

void parser_action(InstructionList** instructions) {
  Token* token = currToken;
  Actions* action = NULL;
  Location* location = NULL;
  Item* item = NULL;
  char numItems = 0;

  parser_accept(TOK_VERB);
  parser_accept(TOK_PREPOSITION);

  if (parser_peek(TOK_WORD) || parser_peek(TOK_PRONOUN)) {
    parser_item();
    if (parser_accept(TOK_CONJUNCTION)) {
      parser_item();
    }
  }

  SGLIB_LIST_REVERSE(NameList, parser_name_stack, next);
  SGLIB_LIST_LEN(NameList, parser_name_stack, next, numItems);

  if (numItems > 0) {
    //ref to first item in parser_name_stack will now be at head
    item = findItemByName(parser_name_stack->name);
    location = findLocationByName(parser_name_stack->name);

    if (item != NULL || location != NULL) {
      action = findActionByNameAndItem(actions, item, parser_action_reg);
      if (action == NULL) {
        action = findDefaultActionByName(actions, parser_action_reg);
      }
    } else {
      parser_error(CC_ITEM_EXPECTED, parser_name_stack->name);
    }
  } else {
    action = findActionByName(actions, parser_action_reg);
  }

  if (action != NULL) {
    lastInstruction = inst_insert(instructions, action->instructions, lastInstruction, numItems > 0 ? parser_name_stack->name : NULL, numItems > 1 ? parser_name_stack->next->name : NULL);
    if (lastInstruction == NULL) {
      create_error(CC_NO_MEMORY, action->name);
      ERR = SE_TERMINAL;
    }
  } else {
    parser_error(CC_NO_SUCH_ACTION, token->val);
  }
}

void parser_command(InstructionList** instructions) {
  parser_empty_name_list();

  if (parser_peek(TOK_VERB)) {
    parser_action(instructions);
  } else if (parser_accept(TOK_QUIT)) {
    parser_quit();
  } else {
    parser_error(CC_ERR_COMMAND_EXPECTED, currToken->val);
  }
}

void parser_commands(InstructionList** instructions) {
  parser_command(instructions);

  if (parser_accept(TOK_EOL)) {
    parser_eol();
  } else if (parser_accept(TOK_COMPLEX)) {
    parser_commands(instructions);
  } else {
    parser_error(CC_ERR_END_OF_COMMAND_EXPECTED, currToken->val);
  }
}

void parser_quit() {
  parser_error(CC_QUIT, currToken->val);
}

void parser_eol() {
  return;
}

enum RunState parse(Token** tokenHead, InstructionList** instructions) {
  currToken = *tokenHead;
  lastInstruction = *instructions;

  ERR = SE_OK;
  parser_counter = 0;

  while(currToken != NULL && ERR == SE_OK) {
    parser_commands(instructions);
  }

  return ERR;
}
