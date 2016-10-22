#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/utils.h"
#include "../include/instruction.h"

extern Location* currentLocation;
extern enum RunState RUNSTATE;

enum Instruction inst_get_instruction_code(char* instructionStr) {
  char instruction[3];

  strncpy(instruction, instructionStr, 2);
  toLowerCase(instruction);
  instruction[2] = '\0';

  if (strComp(instruction, "eq")) {
    return INST_EQ;
  } else if (strComp(instruction, "sp")) {
    return INST_SET_PARAMS;
  } else if (strComp(instruction, "li")) {
    return INST_ITEMINLOCATION;
  } else if (strComp(instruction, "ii")) {
    return INST_ITEMININVENTORY;
  } else if (strComp(instruction, "ic")) {
    return INST_ITEMINCONTEXT;
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
  } else if (strComp(instruction, "gi")) {
    return INST_GOTO_IF;
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
  } else if (strComp(instruction, "pa")) {
    return INST_PRINTACTIONS;
  } else if (strComp(instruction, "pl")) {
    return INST_NEWLINE;
  } else if (strComp(instruction, "ac")) {
    return INST_ACTION;
  }

  return INST_INVALID;
}

void free_instructions(InstructionList* instructions) {
  InstructionList* instruction = instructions;

  SGLIB_LIST_MAP_ON_ELEMENTS(InstructionList, instructions, instruction, next, {
    if (instruction->arg1 != NULL) {
      free(instruction->arg1);
    }
    if (instruction->arg2 != NULL) {
      free(instruction->arg2);
    }
    free(instruction);
  });
}

char* inst_create_arg(char* val) {
  char* arg;
  int len = strlen(val);

  arg = malloc(len + 1);
  if (arg == NULL) {
    return NULL;
  }

  strncpy(arg, val, len);
  arg[len] = '\0';

  return arg;
}

InstructionList* inst_create(char* instructionStr) {
  InstructionList* instruction;
  static char tmpStr[MAX_INST_ARG_SIZE];
  char *first_comma = NULL;
  char *second_comma = NULL;

  memset(tmpStr, 0, MAX_INST_ARG_SIZE);
  
  instruction = malloc(sizeof(struct InstructionList));
  if (instruction == NULL) {
    create_error(SE_TERMINAL, ERR_OUT_OF_MEMORY, instructionStr);
    return NULL;
  }

  instruction->fn = inst_get_instruction_code(instructionStr);
  instruction->arg1 = NULL;
  instruction->arg2 = NULL;
  instruction->next = NULL;

  first_comma = strchr(instructionStr, CON_SPLIT_ARG_CHAR);

  if (first_comma != NULL) {
    first_comma += 1;
    second_comma = strchr(first_comma, CON_SPLIT_ARG_CHAR);

    if (second_comma != NULL) {
      second_comma += 1;

      strncpy(tmpStr, first_comma, second_comma - first_comma - 1);
      instruction->arg1 = inst_create_arg(tmpStr);

      memset(tmpStr, 0, MAX_INST_ARG_SIZE);

      strncpy(tmpStr, second_comma, strlen(second_comma));
      instruction->arg2 = inst_create_arg(tmpStr);
    } else {
      strncpy(tmpStr, first_comma, strlen(first_comma));

      instruction->arg1 = inst_create_arg(tmpStr);
    }
  }

  return instruction;
}

InstructionList* inst_insert(InstructionList** instructions, char* newInstructions, InstructionList* last, char* direct, char* indirect) {
  char* instructionStr;
  static char tmpStr[MAX_INSTRUCTION_LENGTH];
  InstructionList* instruction = NULL;
  char fnStr[4];

  //create an instruction to set special variables $SO and $O here
  sprintf(tmpStr, "SP,%s,%s", direct, indirect);
  instruction = inst_create(tmpStr);
  if (instruction != NULL) {
    SGLIB_LIST_ADD_AFTER(InstructionList, last, instruction, next);
    last = instruction;
  }

  strcpy(tmpStr, newInstructions);
  instructionStr = strtok(tmpStr, CON_SPLIT_INSTR_CHAR);

  while(instructionStr != NULL) {
    instruction = inst_create(instructionStr);
    if (instruction == NULL) {
      sprintf(fnStr, "%s", instruction->fn);
      create_error(SE_TERMINAL, ERR_OUT_OF_MEMORY, fnStr);
      return NULL;
    }
    SGLIB_LIST_ADD_AFTER(InstructionList, last, instruction, next);
    if (*instructions == NULL) {
      *instructions = last;
    }
    last = instruction;
    instructionStr = strtok(NULL, CON_SPLIT_INSTR_CHAR);
  }

  return last;
}