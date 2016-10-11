#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/items.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/instruction.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

extern ErrorList* errorList;
extern Location* currentLocation;
extern Actions* actions;

int equalityRegister = 0;
int skip = SKIP_NONE;

char gotoLabel[MAX_INST_ARG_SIZE];

InstructionList* currInstruction = NULL;

enum RunState ERR = SE_OK;

void intrpt_convertSpecialVariable(char* result, char* arg) {
  if (toLowerCaseCompare(arg, "$l")) {
    strcpy(result, currentLocation->name);
  } else {
    strcpy(result, arg);
  }
}

void intrpt_action(char* output, InstructionList* instructions, char* arg1, char* arg2) {
  Actions* action;
  InstructionList* lastInstruction;
  char actionId;
  char *first_comma = strchr(arg2, ',');

  actionId = atoi(arg1);

  if (first_comma) {
    strncpy(arg1, arg2, first_comma - arg2);
    strcpy(arg2, first_comma + 1);
  } else {
    strcpy(arg1, arg2);
    arg2[0] = '\0';
  }

  action = findActionById(actions, actionId);

  intrpt_convertSpecialVariable(arg1, arg1);
  intrpt_convertSpecialVariable(arg2, arg2);

  if (action != NULL) {
    lastInstruction = inst_insert(&instructions, action->instructions, currInstruction, arg1, arg2);
    if (lastInstruction == NULL) {
      ERR = SE_TERMINAL;
    }
  } else {
    sprintf(output, "%sNO SUCH ACTION: %d\r\n", output, actionId);
  }
}

void intrpt_invalid(char* output, enum Instruction fn, char* arg1, char* arg2) {
  sprintf(output, "%sINVALID INSTRUCTION: %d %s %s\r\n", output, fn, arg1, arg2);
}

void intrpt_eq(char* arg1, char* arg2) {
  equalityRegister = toLowerCaseCompare(arg1, arg2);
}

void intrpt_locationhasitem(char* arg1, char* arg2) {
  Location* loc = findLocationByName(arg1);
  Item* item;

  if (loc != NULL) {
    item = findItemInList(loc->items, arg2);
    if (item != NULL) {
      equalityRegister = 1;
      return;
    }
  }

  equalityRegister = 0;
}

void intrpt_inventoryhasitem(char* arg1) {
  char i = inventoryHasItem(arg1);

  if (i == 1) {
    equalityRegister = 1;
  }

  equalityRegister = 0;
}

void intrpt_hasexit(char* arg1, char* arg2) {
  char i = locationHasExit(arg1, arg2);
  equalityRegister = i;
}

void intrpt_not() {
  equalityRegister = !equalityRegister;
}

void intrpt_if() {
  if (equalityRegister == 0) {
    skip = SKIP_ONE;
  } else {
    skip = SKIP_NONE;
  }
}

void intrpt_setloc(char* arg1) {
  currentLocation = findLocationByName(arg1);
}

void intrpt_additem(char* arg1, char* arg2) {

}

void intrpt_delitem(char* arg1, char* arg2) {

}

void intrpt_return() {
  skip = SKIP_END;
}

void intrpt_goto(char* arg1) {
  skip = SKIP_GOTO;
  strcpy(gotoLabel, arg1);
}

void intrpt_label() {
  skip = SKIP_NONE;
}

void intrpt_print(char* output, char* arg1) {
  sprintf(output, "%s%s", output, arg1);
}

void intrpt_printdesc(char* output, char* arg1) {
  Location* loc = findLocationByName(arg1);
  Item* item;

  if (loc != NULL) {
    sprintf(output, "%s%s", output, loc->description);
  } else {
    item = findItemByName(arg1);
    if (item != NULL) {
      sprintf(output, "%s%s", output, item->description);
    }
  }
}

void intrpt_printexits(char* output, char* arg1) {
  char exits[MAXEXITSTEXTLENGTH];
  Location* loc = findLocationByName(arg1);

  memset(exits, 0, MAXEXITSTEXTLENGTH);

  if (loc != NULL) {
    getAllLocationNames(currentLocation->exits, exits);
    sprintf(output, "%s%s", output, exits);
  } else {
    printOutput("NO SUCH LOCATION %s\r\n", arg1);
  }
}

void intrpt_printitems(char* output, char* arg1) {
  char itemNames[MAXITEMSTEXTLENGTH];
  Location* loc = findLocationByName(arg1);

  memset(itemNames, 0, MAXITEMSTEXTLENGTH);

  if (loc != NULL) {
    getAllItemNames(currentLocation->items, itemNames);
    sprintf(output, "%s%s", output, itemNames);
  } else {
    printOutput("NO SUCH LOCATION %s\r\n", arg1);
  }
}

void intrpt_newline(char* output) {
  sprintf(output, "%s\r\n", output);
}

void intrpt_instruction(char* output, InstructionList* instructions, InstructionList* instruction) {
  enum Instruction fn = instruction->fn;
  char* arg1 = malloc(MAX_INST_ARG_SIZE);
  char* arg2 = malloc(MAX_INST_ARG_SIZE);

  if (fn != INST_ACTION) {
    intrpt_convertSpecialVariable(arg1, instruction->arg1);
    intrpt_convertSpecialVariable(arg2, instruction->arg2);
  } else {
    strcpy(arg1, instruction->arg1);
    strcpy(arg2, instruction->arg2);
  }

  switch (fn) {
    case INST_INVALID:
      intrpt_invalid(output, fn, arg1, arg2);
      break;
    case INST_EQ:
      intrpt_eq(arg1, arg2);
      break;
    case INST_ITEMINLOCATION:
      intrpt_locationhasitem(arg1, arg2);
      break;
    case INST_ITEMININVENTORY:
      intrpt_inventoryhasitem(arg1);
      break;
    case INST_HASEXIT:
      intrpt_hasexit(arg1, arg2);
      break;
    case INST_NOT:
      intrpt_not();
      break;
    case INST_IF:
      intrpt_if();
      break;
    case INST_SETLOC:
      intrpt_setloc(arg1);
      break;
    case INST_ADDITEM:
      intrpt_additem(arg1, arg2);
      break;
    case INST_DELITEM:
      intrpt_delitem(arg1, arg2);
      break;
    case INST_RETURN:
      intrpt_return();
      break;
    case INST_GOTO:
      intrpt_goto(arg1);
      break;
    case INST_LABEL:
      intrpt_label();
      break;
    case INST_PRINT:
      intrpt_print(output, arg1);
      break;
    case INST_PRINTDESC:
      intrpt_printdesc(output, arg1);
      break;
    case INST_PRINTEXITS:
      intrpt_printexits(output, arg1);
      break;
    case INST_PRINTITEMS:
      intrpt_printitems(output, arg1);
      break;
    case INST_NEWLINE:
      intrpt_newline(output);
      break;
    case INST_ACTION:
      intrpt_action(output, instructions, arg1, arg2);
      break;
    default:
      sprintf(output, "%sUNKOWN TOKEN: %d %s %s\r\n", output, fn, arg1, arg2);
  }

  free(arg1);
  free(arg2);
}

enum RunState interpret(InstructionList* instructions, char* output) {
  skip = SKIP_NONE;

  currInstruction = instructions;

  while (currInstruction != NULL && ERR == SE_OK) {
    if (skip == SKIP_GOTO &&
      currInstruction->fn == INST_LABEL &&
      toLowerCaseCompare(gotoLabel, currInstruction->arg1))
    {
      skip = SKIP_NONE;
    } else if (skip == SKIP_ONE) {
      skip = SKIP_NONE;
    } else if (skip == SKIP_NONE) {
      intrpt_instruction(output, instructions, currInstruction);
    }

    currInstruction = currInstruction->next;
  }

  return ERR;
}
