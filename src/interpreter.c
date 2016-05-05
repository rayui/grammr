#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/locations.h"
#include "../include/items.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/instruction.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

extern InstructionList* instructionList;
extern ErrorList* errorList;
extern Location* currentLocation;
extern Location* locations;
extern Item* items;

int equalityRegister = 0;
int skip = SKIP_NONE;

char gotoLabel[MAXARGSIZE];
char arg1[MAXARGSIZE];
char arg2[MAXARGSIZE];

InstructionList* currInstruction = NULL;

void inst_convertSpecialVariable(char* result, char* arg) {
  if (toLowerCaseCompare(arg, "$l")) {
    strcpy(result, currentLocation->name);
  } else {
    strcpy(result, arg);
  }
}

void inst_create_instructions(char* output, char* arg1, char* arg2) {
  Actions* action;
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

  action = findActionById(actionId);

  inst_convertSpecialVariable(arg1, arg1);
  inst_convertSpecialVariable(arg2, arg2);

  if (action != NULL) {
    cc_push_instructions(action->instructions, currInstruction, arg1, arg2);
  } else {
    sprintf(output, "%sNO SUCH ACTION: %d\r\n", output, actionId);
  }
}

void inst_invalid(char* output, enum Instruction fn, char* arg1, char* arg2) {
  sprintf(output, "%sINVALID INSTRUCTION: %d %s %s\r\n", output, fn, arg1, arg2);
}

void inst_eq(char* arg1, char* arg2) {
  equalityRegister = toLowerCaseCompare(arg1, arg2);
}

void inst_locationhasitem(char* arg1, char* arg2) {
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

void inst_inventoryhasitem(char* arg1) {
  char i = inventoryHasItem(arg1);

  if (i == 1) {
    equalityRegister = 1;
  }

  equalityRegister = 0;
}

void inst_hasexit(char* arg1, char* arg2) {
  char i = locationHasExit(arg1, arg2);
  equalityRegister = i;
}

void inst_not() {
  equalityRegister = !equalityRegister;
}

void inst_if() {
  if (equalityRegister == 0) {
    skip = SKIP_ONE;
  } else {
    skip = SKIP_NONE;
  }
}

void inst_setloc(char* arg1) {
  currentLocation = findLocationByName(arg1);
}

void inst_additem(char* arg1, char* arg2) {

}

void inst_delitem(char* arg1, char* arg2) {

}

void inst_return() {
  skip = SKIP_END;
}

void inst_goto(char* arg1) {
  skip = SKIP_GOTO;
  strcpy(gotoLabel, arg1);
}

void inst_label() {
  skip = SKIP_NONE;
}

void inst_print(char* output, char* arg1) {
  sprintf(output, "%s%s", output, arg1);
}

void inst_printdesc(char* output, char* arg1) {
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

void inst_printexits(char* output, char* arg1) {
  char exits[MAXEXITSTEXTLENGTH];
  Location* loc = findLocationByName(arg1);

  memset(exits, 0, MAXEXITSTEXTLENGTH);

  if (loc != NULL) {
    getAllLocationNames(currentLocation->exits, exits);
    sprintf(output, "%s%s", output, exits);
  } else {
    PRINT("NO SUCH LOCATION %s\r\n", arg1);
  }
}

void inst_printitems(char* output, char* arg1) {
  char itemNames[MAXITEMSTEXTLENGTH];
  Location* loc = findLocationByName(arg1);

  memset(itemNames, 0, MAXITEMSTEXTLENGTH);

  if (loc != NULL) {
    getAllItemNames(currentLocation->items, itemNames);
    sprintf(output, "%s%s", output, itemNames);
  } else {
    PRINT("NO SUCH LOCATION %s\r\n", arg1);
  }
}

void inst_newline(char* output) {
  sprintf(output, "%s\r\n", output);
}

void inst_action(output, arg1, arg2) {
  inst_create_instructions(output, arg1, arg2);
}

void inst_switchfn(char* output, InstructionList* instruction) {
  enum Instruction fn = instruction->fn;

  if (fn != INST_ACTION) {
    inst_convertSpecialVariable(arg1, instruction->arg1);
    inst_convertSpecialVariable(arg2, instruction->arg2);
  } else {
    strcpy(arg1, instruction->arg1);
    strcpy(arg2, instruction->arg2);
  }

  switch (fn) {
    case INST_INVALID:
      inst_invalid(output, fn, arg1, arg2);
      break;
    case INST_EQ:
      inst_eq(arg1, arg2);
      break;
    case INST_ITEMINLOCATION:
      inst_locationhasitem(arg1, arg2);
      break;
    case INST_ITEMININVENTORY:
      inst_inventoryhasitem(arg1);
      break;
    case INST_HASEXIT:
      inst_hasexit(arg1, arg2);
      break;
    case INST_NOT:
      inst_not();
      break;
    case INST_IF:
      inst_if();
      break;
    case INST_SETLOC:
      inst_setloc(arg1);
      break;
    case INST_ADDITEM:
      inst_additem(arg1, arg2);
      break;
    case INST_DELITEM:
      inst_delitem(arg1, arg2);
      break;
    case INST_RETURN:
      inst_return();
      break;
    case INST_GOTO:
      inst_goto(arg1);
      break;
    case INST_LABEL:
      inst_label();
      break;
    case INST_PRINT:
      inst_print(output, arg1);
      break;
    case INST_PRINTDESC:
      inst_printdesc(output, arg1);
      break;
    case INST_PRINTEXITS:
      inst_printexits(output, arg1);
      break;
    case INST_PRINTITEMS:
      inst_printitems(output, arg1);
      break;
    case INST_NEWLINE:
      inst_newline(output);
      break;
    case INST_ACTION:
      inst_action(output, arg1, arg2);
      break;
    default:
      sprintf(output, "%sUNKOWN TOKEN: %d %s %s\r\n", output, fn, arg1, arg2);
  }
}

enum RunState interpret(char* output) {
  enum RunState ERR = SE_OK;
  skip = SKIP_NONE;

  currInstruction = instructionList;

  while (currInstruction != NULL) {
    if (skip == SKIP_GOTO &&
      currInstruction->fn == INST_LABEL &&
      toLowerCaseCompare(gotoLabel, currInstruction->arg1))
    {
      skip = SKIP_NONE;
    } else if (skip == SKIP_ONE) {
      skip = SKIP_NONE;
    } else if (skip == SKIP_NONE) {
      inst_switchfn(output, currInstruction);
    }

    currInstruction = currInstruction->next;
  }

  return ERR;
}
