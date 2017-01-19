#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/main.h"
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
char gotoLabel[DEFAULTSTRINGSIZE];
char subject[DEFAULTSTRINGSIZE];
char object[DEFAULTSTRINGSIZE];

Instruction* currentInstruction = NULL;

char stackPointer;
Instruction* stack[5] = {NULL};

void intprt_push_to_stack(Instruction* instruction) {
  stack[stackPointer] = instruction;
  stackPointer++;

  if (stackPointer > 5) {
    create_error(SE_TERMINAL, ERR_STACK_OVERFLOW, "");
  }
}

Instruction* intprt_pop_from_stack(void) {
  Instruction* instruction = NULL;

  if (stackPointer > 0) {
    stackPointer--;
  }

  instruction = stack[stackPointer];
  stack[stackPointer] = NULL;  

  return instruction;
}

void intrprt_error(enum ErrorType error, char* val) {
  create_error(SE_WARN, error, val);
}

char intrpt_in_context(char* name) {
  if (
      inventoryHasItem(name) ||
      findItemInList(currentLocation->items, name) ||
      locationHasExit(currentLocation->name, name) ||
      toLowerCaseCompare(name, currentLocation->name)
  ) {
    return 1;
  }
  return 0;
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
  memset(subject, 0, DEFAULTSTRINGSIZE);
  memset(object, 0, DEFAULTSTRINGSIZE);

  if (arg1 != NULL) {
    strcpy(subject, arg1);
  }

  if (arg2 != NULL) {
    strcpy(object, arg2);
  }
}

void intrpt_action(char* actionIDStr, char* args) {
  Actions* action;
  char *first_comma;

  //find action by first arg (action id)
  action = findActionById(actions, atoi(actionIDStr));

  //if we find an action
  if (action) {
    //args can be NULL, a string, or a string containing a comma
    //the comma is optional and delimits two potential arguments

    if (args) {
      memset(subject, 0, DEFAULTSTRINGSIZE);
      first_comma = strchr(args, ',');
      if (first_comma) {
        memset(object, 0, DEFAULTSTRINGSIZE);
        strncpy(subject, args, first_comma - args);
        strcpy(object, first_comma + 1);
      } else {
        strcpy(subject, args);
      }
    }

    intprt_push_to_stack(currentInstruction->next);
    currentInstruction = action->instructions;
    
  } else {
    intrprt_error(ERR_NO_SUCH_ACTION, actionIDStr);
  }  
}

void intrpt_debug(char* state) {
  char debug_disabled = toLowerCaseCompare(state, "off");
  if (debug_disabled) {
    RUNSTATE = SE_OK;
  }
  RUNSTATE = SE_DEBUG;
}

void intrpt_invalid(enum InstructionType fn) {
  char fnStr[4];
  sprintf(fnStr, "%s", fn);

  intrprt_error(ERR_INVALID_INSTRUCTION, fnStr);
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
  equalityRegister = intrpt_in_context(arg1);
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
  Item* item = findItemInList(currentLocation->items, arg1);

  if (item != NULL && toLowerCaseCompare(arg2, "$i")) {
    deleteItemList(&(currentLocation->items), item);
    createItemList(&inventory, item);
  } else if (inventoryHasItem(arg1)) {
    intrprt_error(ERR_ITEM_IN_INVENTORY, arg1);
  } else {
    intrprt_error(ERR_ITEM_NOT_FOUND, arg1);
  }
}

void intrpt_delitem(char* arg1, char* arg2) {
  Item* item;

  if (toLowerCaseCompare(arg2, "$i") && inventoryHasItem(arg1)) {
    item = findItemInList(inventory, arg1);
    deleteItemList(&inventory, item);
    createItemList(&(currentLocation->items), item);
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
  char* tmpStr = malloc(DEFAULTSTRINGSIZE);
  strcpy(tmpStr, arg1);

  if (arg1 != NULL) {
    if (subject)
      replace_str(tmpStr, "$S", subject);
    if (object) {
      replace_str(tmpStr, "$O", object);
    }
    replace_str(tmpStr, "$L", currentLocation->name);
  }

  sprintf(output, "%s%s", output, tmpStr);

  free(tmpStr);
}

void intrpt_printdesc(char* output, char* arg1) {
  Item* item = intrpt_item_in_context(arg1);
  Location* loc = intrpt_location_in_context(arg1);

  if (item) {
    sprintf(output, "%s\r\n%s\r\n", output, item->description);
  } else if (loc) {
    sprintf(output, "%s\r\n%s\r\n", output, loc->description);  
  } else {
    intrprt_error(ERR_ITEM_NOT_FOUND, arg1);
  }
}

void intrpt_printexits(char* output, char* arg1) {
  char exits[DEFAULTSTRINGSIZE];
  Location* loc = findLocationByName(arg1);

  memset(exits, 0, DEFAULTSTRINGSIZE);

  if (loc != NULL) {
    getAllLocationNames(currentLocation->exits, exits);
    sprintf(output, "%s%s", output, exits);
  } else {
    intrprt_error(ERR_LOCATION_NOT_FOUND, arg1);
  }
}

void intrpt_printitems(char* output, char* arg1) {
  char itemNames[DEFAULTSTRINGSIZE];
  Location* loc = findLocationByName(arg1);

  memset(itemNames, 0, DEFAULTSTRINGSIZE);

  if (loc != NULL) {
    getAllItemNames(currentLocation->items, itemNames);
    sprintf(output, "%s%s", output, itemNames);
  } else {
    intrprt_error(ERR_LOCATION_NOT_FOUND, arg1);
  }
}

void intrpt_printactions(char* output, char* arg1) {
  char actionNames[DEFAULTSTRINGSIZE];
  Item* item = findItemByName(arg1);

  memset(actionNames, 0, DEFAULTSTRINGSIZE);

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

void intrpt_instruction(char* output, Instruction* instruction) {
  enum Instruction fn = instruction->fn;
  char* arg1 = instruction->arg1;
  char* arg2 = instruction->arg2;

  if (fn != INST_SET_PARAMS) {
    arg1 = intrpt_convert_special_variable(arg1);
    arg2 = intrpt_convert_special_variable(arg2);
  }

  printClock(CLOCK);
  printSpinner(CLOCK);

  if (RUNSTATE == SE_DEBUG) {
    printInstruction(equalityRegister, fn, currentLocation->name, subject, object, arg1, arg2);
    cgetc();
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
      intrpt_action(arg1, arg2);
      break;
    case INST_DEBUG:
      intrpt_debug(arg1);
      break;      
    default:
      intrpt_invalid(fn);
  }
  
  CLOCK++;
}

void interpret(char* arg1, char* arg2, Instruction* instructions, char* output) {
  char next = 0;
  skip = SKIP_NONE;
  currentInstruction = instructions;
  stackPointer = 0;

  strcpy(subject, arg1);
  strcpy(object, arg2);

  while (currentInstruction != NULL
    && (
      RUNSTATE == SE_OK ||
      RUNSTATE == SE_DEBUG
    )
  ) {
    next = (currentInstruction->fn != INST_ACTION);

    if (skip == SKIP_GOTO &&
      currentInstruction->fn == INST_LABEL &&
      toLowerCaseCompare(gotoLabel, currentInstruction->arg1))
    {
      skip = SKIP_NONE;
    } else if (skip == SKIP_ONE) {
      skip = SKIP_NONE;
    } else if (skip == SKIP_NONE) {
      intrpt_instruction(output, currentInstruction);
    }

    if (next)
      currentInstruction = currentInstruction->next;
    if (currentInstruction == NULL)
      currentInstruction = intprt_pop_from_stack();
    
  }
}
