#include "../include/instruction.h"

#ifndef ACTIONS
#define ACTIONS

typedef struct Actions {
  char id;
  char* name;
  char isDefault;
  Instruction* instructions;
  struct Actions* next;
} Actions;

Actions* createAction(char id, char* name, char isDefault, char* instructions);
Actions* findActionById(Actions* actions, char id);
Actions* findActionByName(Actions* actions, char* name);
Actions* findDefaultAction(Actions* actions, char* name, char arity);
Actions* findItemAction(Actions* actions, char* actionsArray, char* name, char arity);
char _getArity(Actions* action);
void getAllActionNamesForItem(Actions* actions, struct Item* item, char* actionNames);

#endif