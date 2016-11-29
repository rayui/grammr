#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/items.h"
#include "../include/actions.h"

Actions* createAction(char id, char* name, char isDefault, char* instructions) {
  Actions* action = malloc(sizeof(struct Actions));
  if (action == NULL) {
    return NULL;
  }

  toLowerCase(name);

  action->id = id;
  action->name = name;
  action->instructions = instructions;
  action->isDefault = isDefault;
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
  char i = 0;
  char* actionsArray = item->actions;
  char actionName[MAX_ACTION_SIZE] = {0};
  Actions* action;

  if (actionsArray == NULL)
    return NULL;

  for (i = 1; i <= actionsArray[0]; i++) {
    if (actionsArray[i] > 0) {
      action = findActionById(actions, actionsArray[i]);
      if (toLowerCaseCompare(action->name, name)) {
        return action;
      }
    }
  }

  return NULL;
}

void getAllActionNamesForItem(Actions* actions, Item* item, char* actionNames) {
  char i = 0;
  Actions* action;

  if (item->actions == NULL)
    return;
  
  for (i = 1; i <= item->actions[0]; i++) {
    if (item->actions[i] > 0) {
      action = findActionById(actions, item->actions[i]);
      if (action != NULL) {
        if (i > 0) {
          strcat(actionNames, ", ");
        }
        strcat(actionNames, action->name);
      }
    }
  }

}