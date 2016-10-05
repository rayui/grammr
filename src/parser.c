#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/io.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/instruction.h"
#include "../include/lexer.h"
#include "../include/parser.h"

enum RunState ERR = SE_OK;
int cc_counter = 0;
char cc_word_reg[MAXNAMESZ];
char cc_action_reg[MAXNAMESZ];
NameList* cc_name_stack;
Token* tokenList;
InstructionList* lastInstruction;

extern Item* items;
extern Actions* actions;

void cc_readtok() {
  tokenList = tokenList->next;
  cc_counter++;
}

void cc_set_action_reg() {
  memset(cc_action_reg, 0, MAXNAMESZ);
  strcpy(cc_action_reg, tokenList->val);
}

void cc_concat_action_reg(char* split) {
  strcat(cc_action_reg, split);
  strcat(cc_action_reg, tokenList->val);
}

void cc_set_word_reg() {
  if (strlen(cc_word_reg)) {
    strcat(cc_word_reg, " ");
  }
  strcat(cc_word_reg, tokenList->val);
}

void cc_empty_word_reg() {
  memset(cc_word_reg, 0, MAXNAMESZ);
}

void cc_empty_name_list() {
  SGLIB_LIST_MAP_ON_ELEMENTS(NameList, cc_name_stack, mappedName, next, {
    SGLIB_LIST_DELETE(NameList, cc_name_stack, mappedName, next);
    free(mappedName);
  });
}

void cc_push_word_reg_to_item_reg() {
  struct NameList* newName = malloc(sizeof(struct NameList));
  strcpy(newName->name, cc_word_reg);

  SGLIB_LIST_ADD(struct NameList, cc_name_stack, newName, next);

  cc_empty_word_reg();
}

int cc_peek(enum TokenType type) {
  if (tokenList->type == type) {
    return 1;
  }
  return 0;
}

int cc_peekVal(char *s) {
  return (strcmp(tokenList->val, s) == 0);
}

int cc_accept(enum TokenType type) {
  if (cc_peek(type)) {
    switch (type) {
      case TOK_WORD:
        cc_set_word_reg();
        break;
      case TOK_VERB:
        cc_set_action_reg();
        break;
      case TOK_PREPOSITION:
      case TOK_CONJUNCTION:
        cc_concat_action_reg(" ");
        break;
    }
    cc_readtok();
    return 1;
  }
  return 0;
}

void cc_error(enum ErrorType error, char* val) {
  ERR = SE_WARNING;

  create_error(error, val);

  cc_readtok();
}

void cc_item() {
  cc_accept(TOK_PRONOUN);
  if (cc_peek(TOK_WORD)) {
    while (cc_peek(TOK_WORD)) {
      cc_accept(TOK_WORD);
    }
    cc_push_word_reg_to_item_reg();
  } else {
    cc_error(CC_ITEM_EXPECTED, tokenList->val);
  }
}

void cc_action(InstructionList** instructions) {
  Token* token = tokenList;
  Actions* action = NULL;
  Location* location = NULL;
  Item* item = NULL;
  char numItems = 0;

  cc_accept(TOK_VERB);
  cc_accept(TOK_PREPOSITION);

  if (cc_peek(TOK_WORD) || cc_peek(TOK_PRONOUN)) {
    cc_item();
    if (cc_accept(TOK_CONJUNCTION)) {
      cc_item();
    }
  }

  SGLIB_LIST_REVERSE(NameList, cc_name_stack, next);
  SGLIB_LIST_LEN(NameList, cc_name_stack, next, numItems);

  if (numItems > 0) {
    //ref to first item in cc_name_stack will now be at head
    item = findItemByName(cc_name_stack->name);
    location = findLocationByName(cc_name_stack->name);

    if (item != NULL || location != NULL) {
      action = findActionByNameAndItem(actions, item, cc_action_reg);
      if (action == NULL) {
        action = findDefaultActionByName(actions, cc_action_reg);
      }
    } else {
      cc_error(CC_ITEM_EXPECTED, cc_name_stack->name);
    }
  } else {
    action = findActionByName(actions, cc_action_reg);
  }

  if (action != NULL) {
    lastInstruction = cc_push_instructions(instructions, action->instructions, lastInstruction, numItems > 0 ? cc_name_stack->name : NULL, numItems > 1 ? cc_name_stack->next->name : NULL);
  } else {
    cc_error(CC_NO_SUCH_ACTION, token->val);
  }
}

void cc_command(InstructionList** instructions) {
  cc_empty_name_list();

  if (cc_peek(TOK_VERB)) {
    cc_action(instructions);
  } else if (cc_accept(TOK_QUIT)) {
    cc_quit();
  } else {
    cc_error(CC_ERR_COMMAND_EXPECTED, tokenList->val);
  }
}

void cc_commands(InstructionList** instructions) {
  cc_command(instructions);

  if (cc_accept(TOK_EOL)) {
    cc_eol();
  } else if (cc_accept(TOK_COMPLEX)) {
    cc_commands(instructions);
  } else {
    cc_error(CC_ERR_END_OF_COMMAND_EXPECTED, tokenList->val);
  }
}

void cc_quit() {
  cc_error(CC_QUIT, tokenList->val);
}

void cc_eol() {
  return;
}

void free_parser(InstructionList** instructions) {
  SGLIB_DL_LIST_MAP_ON_ELEMENTS(InstructionList, *instructions, instruction, prev, next, {
    SGLIB_DL_LIST_DELETE(InstructionList, *instructions, instruction, prev, next);
    free(instruction);
  });
}

enum RunState parse(Token** tokens, InstructionList** instructions) {
  //gotta remember where we are so we can clean the tokens after!
  Token* firstToken = *tokens;
  tokenList = *tokens;

  ERR = SE_OK;
  cc_counter = 0;
  lastInstruction = *instructions;

  while(tokenList != NULL && ERR == SE_OK) {
    cc_commands(instructions);
  }

  //reset the token start pointer
  tokens = &firstToken;

  return ERR;
}
