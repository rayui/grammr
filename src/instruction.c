#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/items.h"
#include "../include/utils.h"
#include "../include/instruction.h"

enum Instruction inst_get_instruction(char* instructionStr) {
  char instruction[3];

  strncpy(instruction, instructionStr, 2);
  toLowerCase(instruction);
  instruction[3] = '\0';

  if (strComp(instruction, "eq")) {
    return INST_EQ;
  } else if (strComp(instruction, "li")) {
    return INST_ITEMINLOCATION;
  } else if (strComp(instruction, "ii")) {
    return INST_ITEMININVENTORY;
  } else if (strComp(instruction, "he")) {
    return INST_HASEXIT;
  } else if (strComp(instruction, "nt")) {
    return INST_NOT;
  } else if (strComp(instruction, "if")) {
    return INST_IF;
  } else if (strComp(instruction, "ls")) {
    return INST_SETLOC;
  } else if (strComp(instruction, "ia")) {
    return INST_ADDITEM;
  } else if (strComp(instruction, "id")) {
    return INST_DELITEM;
  } else if (strComp(instruction, "rt")) {
    return INST_RETURN;
  } else if (strComp(instruction, "gt")) {
    return INST_GOTO;
  } else if (strComp(instruction, "lb")) {
    return INST_LABEL;
  } else if (strComp(instruction, "pr")) {
    return INST_PRINT;
  } else if (strComp(instruction, "pd")) {
    return INST_PRINTDESC;
  } else if (strComp(instruction, "pe")) {
    return INST_PRINTEXITS;
  } else if (strComp(instruction, "pi")) {
    return INST_PRINTITEMS;
  } else if (strComp(instruction, "pl")) {
    return INST_NEWLINE;
  } else if (strComp(instruction, "ac")) {
    return INST_ACTION;
  }

  return INST_INVALID;
}

char parseNumArgsFromInstructions(char* instructions) {
  char found = 0;

  if (toLowerCaseContains(instructions, "$S")) {
    found++;
  }
  if (toLowerCaseContains(instructions, "$O")) {
    found++;
  }

  return found;
}

InstructionList* createInstructionList(enum Instruction fn, char* arg1, char* arg2) {
  InstructionList* instruction = malloc(sizeof(struct InstructionList));
  if (instruction == NULL) {
    return NULL;
  }

  instruction->fn = fn;
  strcpy(instruction->arg1, arg1);
  strcpy(instruction->arg2, arg2);
  instruction->next = NULL;

  return instruction;
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

InstructionList* cc_create_instruction(char* instructionStr, char* direct, char* indirect) {
  InstructionList* instruction;
  enum Instruction fn;
  char fnName[MAX_INST_ARG_SIZE];
  char arg1[MAX_INST_ARG_SIZE];
  char arg2[MAX_INST_ARG_SIZE];
  char *first_comma = strchr(instructionStr, CON_SPLIT_ARG_CHAR);
  char *second_comma;

  memset(arg1, 0, MAX_INST_ARG_SIZE);
  memset(arg2, 0, MAX_INST_ARG_SIZE);

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

InstructionList* cc_push_instructions(InstructionList** instructions, char* newInstructions, InstructionList* last, char* direct, char* indirect) {
  char* instructionStr;
  char len = strlen(newInstructions);
  char* tmpStr = malloc(len + 1);
  InstructionList* instruction = NULL;

  strncpy(tmpStr, newInstructions, len);
  tmpStr[len] = '\0';

  instructionStr = strtok(tmpStr, CON_SPLIT_INSTR_CHAR);

  while(instructionStr != NULL) {
    instruction = cc_create_instruction(instructionStr, direct, indirect);
    SGLIB_DL_LIST_ADD_AFTER(InstructionList, last, instruction, prev, next);
    if (*instructions == NULL) {
      *instructions = last;
    }
    instructionStr = strtok(NULL, CON_SPLIT_INSTR_CHAR);
    last = instruction;
  }

  free(tmpStr);

  return last;
}