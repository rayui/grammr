#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/instruction.h"
#include "../include/lexer.h"
#include "../include/parser.h"

const char CON_SPLIT_INSTR_CHAR[2] = ";\0";
const char CON_SPLIT_ARG_CHAR = ',';

enum RunState ERR = SE_OK;
int cc_counter = 0;
char cc_word_reg[MAXNAMESZ];
NameList* cc_name_stack;
char cc_action_reg[MAXNAMESZ];

extern InstructionList* instructionList;
extern TokenList* tokenList;
extern ErrorList* errorList;
extern Item* items;
extern Actions* actions;

InstructionList* lastInstruction;

void cc_readtok() {
  tokenList = tokenList->next;
  cc_counter++;
}

void cc_empty_name_list() {
  SGLIB_LIST_MAP_ON_ELEMENTS(NameList, cc_name_stack, mappedName, next, {
    SGLIB_LIST_DELETE(NameList, cc_name_stack, mappedName, next);
    free(mappedName);
  });
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
  memset(cc_word_reg, 0, MAXNAMESZ);
  strcpy(cc_word_reg, tokenList->val);
}

void cc_concat_word_reg() {
  strcat(cc_word_reg, " ");
  strcat(cc_word_reg, tokenList->val);
}

void cc_push_to_cc_word_reg() {
  struct NameList* newName = malloc(sizeof(struct NameList));
  strcpy(newName->name, cc_word_reg);

  SGLIB_LIST_ADD(struct NameList, cc_name_stack, newName, next);
}

void cc_convertSpecialVariable(char* arg, char* direct, char* indirect) {
  char* argRepl;

  if (arg == NULL) return;

  if (direct) {
    argRepl = replace_str(arg, "$S", direct);
    strcpy(arg, argRepl);
  }

  if (indirect) {
    argRepl = replace_str(arg, "$O", indirect);
    strcpy(arg, argRepl);
  }
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
    cc_readtok();
    return 1;
  }
  return 0;
}

int cc_acceptVal(char *s) {
  if (cc_peekVal(s)) {
    cc_readtok();
    return 1;
  }
  return 0;
}

InstructionList* cc_create_instruction(char* instructionStr, char* direct, char* indirect) {
  InstructionList* instruction;
  enum Instruction fn;
  char fnName[MAXARGSIZE];
  char arg1[MAXARGSIZE];
  char arg2[MAXARGSIZE];
  char *first_comma = strchr(instructionStr, CON_SPLIT_ARG_CHAR);
  char *second_comma;

  memset(arg1, 0, MAXARGSIZE);
  memset(arg2, 0, MAXARGSIZE);

  fn = inst_get_instruction(instructionStr);

  if (first_comma) {
    second_comma = strchr(first_comma + 1, CON_SPLIT_ARG_CHAR);
    if (second_comma) {
      strncpy(arg1, first_comma + 1, second_comma - first_comma - 1);
      strcpy(arg2, second_comma + 1);
    } else {
      strcpy(arg1, first_comma + 1);
    }
  } else {
    strcpy(fnName, instructionStr);
  }

  cc_convertSpecialVariable(arg1, direct, indirect);
  cc_convertSpecialVariable(arg2, direct, indirect);

  instruction = createInstructionList(fn, arg1, arg2);

  return instruction;
}

InstructionList* cc_push_instructions(char* instructions, InstructionList* last, char* direct, char* indirect) {
  char* instructionStr;
  char len = strlen(instructions);
  char* tmpStr = malloc(len + 1);
  InstructionList* instruction = NULL;

  strncpy(tmpStr, instructions, len);
  tmpStr[len] = '\0';

  instructionStr = strtok(tmpStr, CON_SPLIT_INSTR_CHAR);

  while(instructionStr != NULL) {
    instruction = cc_create_instruction(instructionStr, direct, indirect);
    SGLIB_DL_LIST_ADD_AFTER(InstructionList, last, instruction, prev, next);
    if (instructionList == NULL) {
      instructionList = last;
    }
    instructionStr = strtok(NULL, CON_SPLIT_INSTR_CHAR);
    last = instruction;
  }

  free(tmpStr);

  return lastInstruction;
}

void cc_error(enum ErrorType error, char* val) {
  ERR = SE_WARNING;

  create_error(error, val);

  cc_readtok();
}

void cc_word() {
  if (cc_peek(TOK_WORD)) {
    cc_set_word_reg();
    cc_accept(TOK_WORD);
  } else {
    cc_error(CC_WORD_EXPECTED, tokenList->val);
  }
}

void cc_item_name() {
  if (cc_peek(TOK_WORD)) {
    cc_concat_word_reg();
    cc_accept(TOK_WORD);
    cc_item_name();
  }
}

void cc_item() {
  if (cc_peek(TOK_WORD)) {
    if (cc_peek(TOK_PRONOUN)) {
      PRINT("PRONOUN!\r\n");
    }
    cc_accept(TOK_PRONOUN);
    cc_word();
    cc_item_name();
    cc_push_to_cc_word_reg();
  } else {
    cc_error(CC_ITEM_EXPECTED, tokenList->val);
  }
}

void cc_location_name() {
  cc_item();
}

void cc_preposition() {
  if (cc_peek(TOK_PREPOSITION)) {
    cc_concat_action_reg(",");
    cc_accept(TOK_PREPOSITION);
  } else {
    cc_error(CC_PREPOSITION_EXPECTED, tokenList->val);
  }
}

void cc_conjunction() {
  if (cc_peek(TOK_CONJUNCTION)) {
    cc_concat_action_reg(",");
    cc_accept(TOK_CONJUNCTION);
  } else {
    cc_error(CC_CONJUNCTION_EXPECTED, tokenList->val);
  }
}

void cc_verb_intrans () {

}

void cc_verb_monotrans () {
  cc_item();
  if (cc_peek(TOK_CONJUNCTION)) {
    cc_conjunction();
    cc_item();
  }
}

void cc_verb_ditrans () {
  cc_item();
  cc_preposition();
  cc_item();
}

void cc_action(char transitivity) {
  TokenList* token = tokenList;
  Actions* action = NULL;
  Location* location = NULL;
  Item* item = NULL;
  char len = 0;

  cc_set_action_reg();
  cc_accept(TOK_VERB);

  switch(transitivity) {
    case 0:
      cc_verb_intrans();
      break;
    case 1:
      cc_verb_monotrans();
      break;
    case 2:
      cc_verb_ditrans();
      break;
    default:
      cc_error(CC_NO_SUCH_ACTION, token->val);
      break;
  }

  SGLIB_LIST_REVERSE(NameList, cc_name_stack, next);
  SGLIB_LIST_LEN(NameList, cc_name_stack, next, len);

  if (transitivity > 0 && len > 0) {
    //ref to first item in cc_name_stack will now be at head
    item = findItemByName(cc_name_stack->name);
    location = findLocationByName(cc_name_stack->name);

    if (item != NULL || location != NULL) {
      action = findActionByNameAndItem(item, cc_action_reg);
      if (action == NULL) {
        action = findDefaultActionByName(cc_action_reg);
      }
    } else {
      cc_error(CC_ITEM_EXPECTED, cc_name_stack->name);
    }
  } else {
    action = findActionByName(cc_action_reg);
  }

  if (action != NULL) {
    lastInstruction = cc_push_instructions(action->instructions, lastInstruction, len > 0 ? cc_name_stack->name : NULL, len > 1 ? cc_name_stack->next->name : NULL);
  } else {
    cc_error(CC_NO_SUCH_ACTION, token->val);
  }
}

void cc_command() {
  cc_empty_name_list();

  if (cc_peek(TOK_VERB)) {
    cc_action(getVerbTransitivityByName(tokenList->val));
  } else if (cc_accept(TOK_QUIT)) {
    cc_quit();
  } else {
    cc_error(CC_ERR_COMMAND_EXPECTED, tokenList->val);
  }
}

void cc_commands() {
  cc_command();

  if (cc_accept(TOK_EOL)) {
    cc_eol();
  } else if (cc_accept(TOK_COMPLEX)) {
    cc_commands();
  } else {
    cc_error(CC_ERR_END_OF_COMMAND_EXPECTED, tokenList->val);
  }
}

void cc_eol() {
  return;
}

void cc_quit() {
  cc_error(CC_QUIT, tokenList->val);
}

void free_parser() {
  SGLIB_DL_LIST_MAP_ON_ELEMENTS(struct InstructionList, instructionList, instruction, prev, next, {
    SGLIB_DL_LIST_DELETE(struct InstructionList, instructionList, instruction, prev, next);
    free(instruction);
  });
}

enum RunState parse() {
  //gotta remember where we are so we can clean the tokens after!
  struct TokenList* tokenStart = tokenList;

  ERR = SE_OK;
  cc_counter = 0;
  lastInstruction = instructionList;

  while(tokenList != NULL && ERR == SE_OK) {
    cc_commands();
  }

  //reset the token start pointer
  tokenList = tokenStart;

  return ERR;
}
