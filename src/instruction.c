#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/items.h"
#include "../include/utils.h"
#include "../include/instruction.h"

extern Actions* actions;

enum Instruction inst_get_instruction(char* instructionStr) {
  char instruction[3];

  strncpy(instruction, instructionStr, 2);
  instruction[3] = '\0';

  if (toLowerCaseCompare(instruction, "eq")) {
    return INST_EQ;
  } else if (toLowerCaseCompare(instruction, "li")) {
    return INST_ITEMINLOCATION;
  } else if (toLowerCaseCompare(instruction, "ii")) {
    return INST_ITEMININVENTORY;
  } else if (toLowerCaseCompare(instruction, "he")) {
    return INST_HASEXIT;
  } else if (toLowerCaseCompare(instruction, "nt")) {
    return INST_NOT;
  } else if (toLowerCaseCompare(instruction, "if")) {
    return INST_IF;
  } else if (toLowerCaseCompare(instruction, "ls")) {
    return INST_SETLOC;
  } else if (toLowerCaseCompare(instruction, "ia")) {
    return INST_ADDITEM;
  } else if (toLowerCaseCompare(instruction, "id")) {
    return INST_DELITEM;
  } else if (toLowerCaseCompare(instruction, "rt")) {
    return INST_RETURN;
  } else if (toLowerCaseCompare(instruction, "gt")) {
    return INST_GOTO;
  } else if (toLowerCaseCompare(instruction, "lb")) {
    return INST_LABEL;
  } else if (toLowerCaseCompare(instruction, "pr")) {
    return INST_PRINT;
  } else if (toLowerCaseCompare(instruction, "pd")) {
    return INST_PRINTDESC;
  } else if (toLowerCaseCompare(instruction, "pe")) {
    return INST_PRINTEXITS;
  } else if (toLowerCaseCompare(instruction, "pi")) {
    return INST_PRINTITEMS;
  } else if (toLowerCaseCompare(instruction, "pl")) {
    return INST_NEWLINE;
  } else if (toLowerCaseCompare(instruction, "ac")) {
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

Actions* createAction(char id, char* name, char* instructions, char isDefault) {
  int instLen = strlen(instructions);

  Actions* action = malloc(sizeof(struct Actions));
  if (action == NULL) {
    return NULL;
  }

  action->id = id;
  strcpy(action->name, name);
  action->instructions = malloc(instLen + 1 * sizeof(char));
  strcpy(action->instructions, instructions);
  action->isDefault = isDefault;
  (action->instructions)[instLen] = 0;
  action->next = NULL;

  return action;
}

Actions* findActionByName(char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (toLowerCaseCompare(action->name, name)) {
      return action;
    }
  });

  return NULL;
}

Actions* findActionById(char id) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (action->id == id) {
      return action;
    }
  });

  return NULL;
}

Actions* findDefaultActionByName(char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (toLowerCaseCompare(action->name, name) && action->isDefault) {
      return action;
    }
  });

  return NULL;
}

Actions* findActionByNameAndItem(struct Item* item, char* name) {
  char i;

  for (i = 0; i < MAXINSTRUCTIONS; i++) {
    SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
      if (action->id == (item->actions)[i]) {
        if (toLowerCaseCompare(action->name, name)) {
          return action;
        }
      }
    });
  }

  return NULL;
}
