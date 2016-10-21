#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/items.h"
#include "../include/actions.h"

Actions* createAction(char id, char* name, char* instructions, char isDefault) {
  int instLen = strlen(instructions);

  Actions* action = malloc(sizeof(struct Actions));
  if (action == NULL) {
    return NULL;
  }

  toLowerCase(name);

  action->id = id;
  strcpy(action->name, name);
  action->instructions = malloc(instLen + 1 * sizeof(char));
  strcpy(action->instructions, instructions);
  action->isDefault = isDefault;
  (action->instructions)[instLen] = 0;
  action->next = NULL;

  return action;
}

Actions* findActionByName(Actions* actions, char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (strComp(action->name, name)) {
      return action;
    }
  });

  return NULL;
}

Actions* findActionById(Actions* actions, char id) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (action->id == id) {
      return action;
    }
  });

  return NULL;
}

Actions* findDefaultActionByName(Actions* actions, char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (strComp(action->name, name) && action->isDefault) {
      return action;
    }
  });

  return NULL;
}

Actions* findActionByNameAndItem(Actions* actions, struct Item* item, char* name) {
  int i = 0;

  SGLIB_LIST_LEN(Actions, actions, next, i);

  while (i--) {
    SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
      if (action->id == (item->actions)[i]) {
        if (strComp(action->name, name)) {
          return action;
        }
      }
    });
  }

  return NULL;
}

void getAllActionNamesForItem(Actions* actions, Item* item, char* actionNames) {
  char i = 0;
  char name[MAX_ACTION_SIZE];
  Actions* action;
  
  for (i = 0; i < MAXINSTRUCTIONS; i++) {
    if ((item->actions)[i] > 0) {
      action = findActionById(actions, (item->actions)[i]);
      if (action != NULL) {
        if (i > 0) {
          strcat(actionNames, ", ");
        }
        strcat(actionNames, action->name);
      }
    }
  }

}