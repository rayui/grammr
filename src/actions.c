#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/main.h"
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
  action->instructions = inst_add(instructions);
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

Actions* findDefaultAction(Actions* actions, char* name, char numArgs) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Actions, actions, action, next, {
    if (strComp(action->name, name) && action->isDefault) {
      return action;
    }
  });

  return NULL;
}


Actions* findItemAction(Actions* actions, char* actionsArray, char* name, char numArgs) {
  char i = 0;
  char actionName[DEFAULTSTRINGSIZE] = {0};
  Actions* action;

  if (actionsArray == NULL)
    return NULL;

  for (i = 1; i <= actionsArray[0]; i++) {
    action = findActionById(actions, actionsArray[i]);
    if (toLowerCaseCompare(action->name, name)) {
      return action;
    }
  }

  return NULL;
}

void getAllActionNamesForItem(Actions* actions, Item* item, char* actionNames) {
  char i;
  Actions* action;

  if (item->actions == NULL)
    return;
  
  for (i = 1; i <= item->actions[0]; i++) {
    action = findActionById(actions, item->actions[i]);
    if (action != NULL) {
      if (i > 1) {
        strcat(actionNames, ", ");
      }
      strcat(actionNames, action->name);
    }
  }

}