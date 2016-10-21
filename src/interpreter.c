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

extern Location* currentLocation;
extern Actions* actions;
extern ItemList* inventory;
extern enum RunState RUNSTATE;
extern long CLOCK;

int equalityRegister = 0;
int skip = SKIP_NONE;
char gotoLabel[MAX_INST_ARG_SIZE];
char subject[MAX_INST_ARG_SIZE];
char object[MAX_INST_ARG_SIZE];

InstructionList* currInstruction = NULL;

void intrprt_error(enum ErrorType error, char* val) {
  create_error(SE_INTRPRT, error, val);
}

Item* intrpt_item_in_context(char* name) {
  if (
      inventoryHasItem(name) ||
      findItemInList(currentLocation->items, name)
      ) {
    return findItemByName(name);
  }
  return NULL;
}

Location* intrpt_location_in_context(char* name) {
  if (
      locationHasExit(currentLocation->name, name) ||
      toLowerCaseCompare(name, currentLocation->name)
      ) {
    return findLocationByName(name);
  }
  return NULL;
}

char* intrpt_convert_special_variable(char* arg) {
  if (toLowerCaseCompare(arg, "$l")) {
    return currentLocation->name;
  } else if (toLowerCaseCompare(arg, "$s")) {
    return subject;
  } else if (toLowerCaseCompare(arg, "$o")) {
    return object;
  }

  return arg;
}

void intrpt_set_params(char* arg1, char* arg2) {
  memset(subject, 0, MAX_INST_ARG_SIZE);
  memset(object, 0, MAX_INST_ARG_SIZE);
  strcpy(subject, arg1);
  strcpy(object, arg2);
}

void intrpt_action(InstructionList* instructions, char* actionIDStr, char* args) {
  Actions* action;
  InstructionList* lastInstruction;
  char *first_comma;
  static char arg1[MAX_INST_ARG_SIZE];
  static char arg2[MAX_INST_ARG_SIZE];

  action = findActionById(actions, atoi(actionIDStr));

  if (action != NULL) {
    strcpy(arg1, actionIDStr);
    strcpy(arg2, args);
    
    first_comma = strchr(arg2, ',');

    if (first_comma) {
      strncpy(arg1, arg2, first_comma - arg2);
      strcpy(arg2, first_comma + 1);
    } else {
      strcpy(arg1, arg2);
    }

    strcpy(arg1, intrpt_convert_special_variable(arg1));
    strcpy(arg2, intrpt_convert_special_variable(arg2));

    lastInstruction = inst_insert(&instructions, action->instructions, currInstruction, arg1, arg2);
    if (lastInstruction == NULL) {
      create_error(SE_TERMINAL, ERR_OUT_OF_MEMORY, action->name);
    }
  } else {
    intrprt_error(ERR_NO_SUCH_ACTION, actionIDStr);
  }  
}

void intrpt_invalid(enum Instruction fn) {
  intrprt_error(ERR_INVALID_INSTRUCTION, fn);
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
  equalityRegister = inventoryHasItem(arg1);
}

void intrprt_itemincontext(char* arg1) {
  Item* item = intrpt_item_in_context(arg1);
  if (item) {
    equalityRegister = 1;
  } else {
    equalityRegister = 0;
  }
}

void intrpt_hasexit(char* arg1, char* arg2) {
  equalityRegister = locationHasExit(arg1, arg2);
}

void intrpt_not() {
  equalityRegister = !equalityRegister;
}

void intrpt_if() {
  if (equalityRegister == 1) {
    skip = SKIP_NONE;
  } else {
    skip = SKIP_ONE; 
  }
}

void intrpt_setloc(char* arg1) {
  currentLocation = findLocationByName(arg1);
}

void intrpt_additem(char* arg1, char* arg2) {
  Item* subject = findItemInList(currentLocation->items, arg1);

  if (subject != NULL && toLowerCaseCompare(arg2, "$i")) {
    deleteItemList(&(currentLocation->items), subject);
    createItemList(&inventory, subject);
  } else if (inventoryHasItem(arg1)) {
    intrprt_error(ERR_ITEM_IN_INVENTORY, arg1);
  } else {
    intrprt_error(ERR_ITEM_NOT_FOUND, arg1);
  }
}

void intrpt_delitem(char* arg1, char* arg2) {
  Item* subject;

  if (toLowerCaseCompare(arg2, "$i") && inventoryHasItem(arg1)) {
    subject = findItemInList(inventory, arg1);
    deleteItemList(&inventory, subject);
    createItemList(&(currentLocation->items), subject);
  } else {
    intrprt_error(ERR_ITEM_NOT_IN_INVENTORY, arg1);
  }
}

void intrpt_return(void) {
  skip = SKIP_END;
}

void intrpt_goto(char* arg1) {
  skip = SKIP_GOTO;
  strcpy(gotoLabel, arg1);
}

void intrpt_goto_if(char* arg1) {
  if (equalityRegister == 1) {
    skip = SKIP_GOTO;
    strcpy(gotoLabel, arg1);
  } else {
    skip = SKIP_NONE;
  }
}

void intrpt_label(void) {
  skip = SKIP_NONE;
}

void intrpt_print(char* output, char* arg1) {
  char* tmpStr;

  if (arg1 != NULL) {
    if (subject)
      tmpStr = replace_str(arg1, "$S", subject);
    if (object) {
      tmpStr = replace_str(tmpStr, "$O", object);
    }
    tmpStr = replace_str(tmpStr, "$L", currentLocation->name);
  }

  sprintf(output, "%s%s", output, tmpStr);
}

void intrpt_printdesc(char* output, char* arg1) {
  Item* item = intrpt_item_in_context(arg1);
  Location* loc = intrpt_location_in_context(arg1);

  if (item) {
    sprintf(output, "%s\r\n%s", output, item->description);
  } else if (loc) {
    sprintf(output, "%s\r\n%s", output, loc->description);  
  } else {
    intrprt_error(ERR_ITEM_NOT_FOUND, arg1);
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
    intrprt_error(ERR_LOCATION_NOT_FOUND, arg1);
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
    intrprt_error(ERR_LOCATION_NOT_FOUND, arg1);
  }
}

void intrpt_printactions(char* output, char* arg1) {
  char actionNames[MAXACTIONSTEXTLENGTH];
  Item* item = findItemByName(arg1);

  memset(actionNames, 0, MAXACTIONSTEXTLENGTH);

  if (item != NULL) {
    getAllActionNamesForItem(actions, item, actionNames);
    sprintf(output, "%s%s", output, actionNames);
  } else {
    intrprt_error(ERR_ACTIONS_NOT_FOUND, arg1);
  }
}

void intrpt_newline(char* output) {
  sprintf(output, "%s\r\n", output);
}

void intrpt_instruction(char* output, InstructionList* instructions, InstructionList* instruction) {
  enum Instruction fn = instruction->fn;
  char* arg1 = instruction->arg1;
  char* arg2 = instruction->arg2;

  if (fn != INST_SET_PARAMS) {
    arg1 = intrpt_convert_special_variable(arg1);
    arg2 = intrpt_convert_special_variable(arg2);
  }

  switch (fn) {
    case INST_INVALID:
      intrpt_invalid(fn);
      break;
    case INST_SET_PARAMS:
      intrpt_set_params(arg1, arg2);
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
    case INST_ITEMINCONTEXT:
      intrprt_itemincontext(arg1);
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
    case INST_GOTO_IF:
      intrpt_goto_if(arg1);
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
    case INST_PRINTACTIONS:
      intrpt_printactions(output, arg1);
      break;
    case INST_NEWLINE:
      intrpt_newline(output);
      break;
    case INST_ACTION:
      intrpt_action(instructions, arg1, arg2);
      break;
    default:
      intrprt_error(ERR_UNKNOWN_INSTRUCTION, fn);
  }

  printInstruction(CLOCK, equalityRegister, fn, arg1 ? arg1 : "NULL", arg2 ? arg2 : "NULL");

  CLOCK++;
}

void interpret(InstructionList** instructions, char* output) {
  skip = SKIP_NONE;
  currInstruction = *instructions;

  while (currInstruction != NULL && RUNSTATE == SE_OK) {
    if (skip == SKIP_GOTO &&
      currInstruction->fn == INST_LABEL &&
      toLowerCaseCompare(gotoLabel, currInstruction->arg1))
    {
      skip = SKIP_NONE;
    } else if (skip == SKIP_ONE) {
      skip = SKIP_NONE;
    } else if (skip == SKIP_NONE) {
      intrpt_instruction(output, *instructions, currInstruction);
    }

    currInstruction = currInstruction->next;
  }
}
