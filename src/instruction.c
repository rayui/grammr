#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/main.h"
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/utils.h"
#include "../include/instruction.h"

extern Location* currentLocation;
extern enum RunState RUNSTATE;

enum InstructionType inst_get_instruction_code(char* instructionStr) {
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
  } else if (strComp(instruction, "db")) {
    return INST_DEBUG;
  }

  return INST_INVALID;
}

void free_instructions(Instruction* instructions) {
  Instruction* instruction = instructions;

  SGLIB_LIST_MAP_ON_ELEMENTS(Instruction, instructions, instruction, next, {
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

Instruction* inst_create(char* instructionStr) {
  Instruction* instruction;
  char tmpStr[DEFAULTSTRINGSIZE] = {0};
  char *first_comma = NULL;
  char *second_comma = NULL;
  char first_arg_len = 0;

  instruction = malloc(sizeof(struct Instruction));
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
      strncpy(tmpStr, second_comma + 1, strlen(second_comma + 1));
      instruction->arg2 = inst_create_arg(tmpStr);
      first_arg_len = second_comma - first_comma;
      memset(tmpStr, 0, first_arg_len + 1);
    } else {
      first_arg_len = strlen(first_comma);
    }

    strncpy(tmpStr, first_comma, first_arg_len);
    instruction->arg1 = inst_create_arg(tmpStr);
  }

  return instruction;
}

Instruction* inst_set_params(Instruction* last, char* direct, char* indirect) {
  char tmpStr[DEFAULTSTRINGSIZE] = {0};
  Instruction* instruction = NULL;

  //create an instruction to set special variables $SO and $O here

  sprintf(tmpStr, "SP");

  if (direct != NULL) {
    sprintf(tmpStr, "%s,%s", tmpStr, direct);
    if (indirect != NULL)
      sprintf(tmpStr, "%s,%s", tmpStr, indirect);
  }
  
  instruction = inst_create(tmpStr);
  if (instruction != NULL) {
    SGLIB_LIST_ADD_AFTER(Instruction, last, instruction, next);
    last = instruction;
  }

  return last;
}

Instruction* inst_insert(char* newInstructions, Instruction* last) {
  char* instructionStr;
  char tmpStr[MAX_INSTRUCTION_LENGTH];
  Instruction* instruction = NULL;
  char fnStr[4];

  strcpy(tmpStr, newInstructions);
  instructionStr = strtok(tmpStr, CON_SPLIT_INSTR_CHAR);

  while(instructionStr != NULL) {
    instruction = inst_create(instructionStr);
    if (instruction != NULL) {
      SGLIB_LIST_ADD_AFTER(Instruction, last, instruction, next);
      last = instruction;
      instructionStr = strtok(NULL, CON_SPLIT_INSTR_CHAR);
    } else {
      return last;
    }
  }

  return last;
}