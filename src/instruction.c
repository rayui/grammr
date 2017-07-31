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
  //figure this out for when we reload config
}

InstructionArg* inst_create_arg(char* val) {
  InstructionArg* arg = malloc(sizeof(struct InstructionArg));
  char len = strlen(val);

  if (arg == NULL) {
    create_error(SE_TERMINAL, ERR_OUT_OF_MEMORY, val);
    return NULL;
  }

  arg->val = val;
  arg->next = NULL;

  return arg;

}

Instruction* inst_create(char* instructionStr) {
  char len = 0;
  char* token = NULL;
  char* running = strdup(instructionStr);

  Instruction* instruction = malloc(sizeof(struct Instruction));
  InstructionArg* arg = NULL;
  InstructionArg* last = NULL;

  if (instruction == NULL) {
    create_error(SE_TERMINAL, ERR_OUT_OF_MEMORY, instructionStr);
    return NULL;
  }

  instruction->arg = NULL;
  instruction->next = NULL;

  token = strsep(&running, ",");
  if (token == NULL) {
    create_error(SE_WARN, ERR_INVALID_INSTRUCTION, instructionStr);
    return NULL;
  }

  instruction->fn = inst_get_instruction_code(instructionStr);

  token = strsep(&running, ",");
  while (token != NULL) {
    arg = inst_create_arg(token);
    SGLIB_LIST_ADD_AFTER(InstructionArg, last, arg, next);
    if (instruction->arg == NULL)
      instruction->arg = arg;
    last = arg;
    token = strsep(&running, ",");
  }

  return instruction;
}

Instruction* inst_add(char* newInstructions) {
  char* instructionStr;
  char tmpStr[MAX_INSTRUCTION_LENGTH];
  char debug[128];
  Instruction* instruction = NULL;
  Instruction* head = NULL;
  Instruction* last = NULL;

  strcpy(tmpStr, newInstructions);
  instructionStr = strtok(tmpStr, CON_SPLIT_INSTR_CHAR);

  while(instructionStr != NULL) {
    instruction = inst_create(instructionStr);
    if (instruction != NULL) {
      SGLIB_LIST_ADD_AFTER(Instructio>fn, last, instruction, next);
      if (head == NULL)
        head = instruction;
      last = instruction;
      instructionStr = strtok(NULL, CON_SPLIT_INSTR_CHAR);
    }
  }

  return head;
}