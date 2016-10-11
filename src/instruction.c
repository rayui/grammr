#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/utils.h"
#include "../include/instruction.h"

enum Instruction inst_get_instruction_code(char* instructionStr) {
  char instruction[3];

  strncpy(instruction, instructionStr, 2);
  toLowerCase(instruction);
  instruction[2] = '\0';

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

void inst_convert_special_variable(char* arg, char* direct, char* indirect) {
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

InstructionList* inst_create(char* instructionStr, char* direct, char* indirect) {
  InstructionList* instruction;
  char *arg1 = malloc(MAX_INST_ARG_SIZE);
  char *arg2 = malloc(MAX_INST_ARG_SIZE);
  char *first_comma = strchr(instructionStr, CON_SPLIT_ARG_CHAR);
  char *second_comma;

  instruction = malloc(sizeof(struct InstructionList));
  if (instruction == NULL ||
      arg1 == NULL ||
      arg2 == NULL) {
    return NULL;
  }

  memset(arg1, 0, MAX_INST_ARG_SIZE);
  memset(arg2, 0, MAX_INST_ARG_SIZE);

  if (first_comma) {
    second_comma = strchr(first_comma + 1, CON_SPLIT_ARG_CHAR);
    if (second_comma) {
      strncpy(arg1, first_comma + 1, second_comma - first_comma - 1);
      strcpy(arg2, second_comma + 1);
    } else {
      strcpy(arg1, first_comma + 1);
    }
  }

  inst_convert_special_variable(arg1, direct, indirect);
  inst_convert_special_variable(arg2, direct, indirect);

  instruction->fn = inst_get_instruction_code(instructionStr);
  strcpy(instruction->arg1, arg1);
  strcpy(instruction->arg2, arg2);
  instruction->next = NULL;

  free(arg1);
  free(arg2);

  return instruction;
}

InstructionList* inst_insert(InstructionList** instructions, char* newInstructions, InstructionList* last, char* direct, char* indirect) {
  char* instructionStr;
  int len = strlen(newInstructions);
  char* tmpStr = malloc(len + 1);
  InstructionList* instruction = NULL;

  strncpy(tmpStr, newInstructions, len);
  tmpStr[len] = '\0';

  instructionStr = strtok(tmpStr, CON_SPLIT_INSTR_CHAR);

  while(instructionStr != NULL) {
    instruction = inst_create(instructionStr, direct, indirect);
    if (instruction == NULL) {
      free(tmpStr);
      return NULL;
    }
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