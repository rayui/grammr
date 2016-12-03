#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/config_parser_new.h"

FILE* fp;
char *source;
extern Item* items;
extern Location* locations;
extern Actions* actions;
extern enum RunState RUNSTATE;

Location* locationsTail;
Actions* actionsTail;

void con_error(enum ErrorType error) {
  create_error(SE_TERMINAL, error, "Error parsing config file");
}

char readOneByte(void) {
  return fgetc(fp);
}

char readNBytes(char* str, char n) {
  char lastByte;
  char i;

  if (str == NULL) {
    con_error(ERR_OUT_OF_MEMORY);
    return NULL;
  }

  for (i = 0; i < n; i++) {
    lastByte = readOneByte();
    if (lastByte == EOF) {
      con_error(ERR_EOF);
      return i;
    }
    str[i] = lastByte;
  }

  toC64Case(str);

  return i;
}

void readItem() {
  char byteLen;
  char id;
  char* name;
  char* desc;
  char* actions;
  Item* item;

  id = readOneByte();

  byteLen = readOneByte();
  name = malloc(byteLen + 1);
  readNBytes(name, byteLen);
  name[byteLen] = 0;

  byteLen = readOneByte();
  desc = malloc(byteLen + 1);
  readNBytes(desc, byteLen);
  desc[byteLen] = 0;

  byteLen = readOneByte();
  actions = malloc(byteLen + 1);
  actions[0] = byteLen;
  readNBytes(actions + 1, byteLen);

  item = createItem(id, name, desc, actions);
  SGLIB_LIST_ADD(Item, items, item, next);
}

void readAction() {
  char byteLen;
  char id;
  char* name;
  char isDefault;
  char* instructions;
  Actions* action;

  id = readOneByte();

  byteLen = readOneByte();
  name = malloc(byteLen + 1);
  readNBytes(name, byteLen);
  name[byteLen] = 0;

  isDefault = readOneByte();

  byteLen = readOneByte();
  instructions = malloc(byteLen + 1);
  readNBytes(instructions, byteLen);
  instructions[byteLen] = 0;

  action = createAction(id, name, isDefault, instructions);
  SGLIB_LIST_ADD_AFTER(Actions, actionsTail, action, next);

  if (actions == NULL) {
    actions = actionsTail;
  }

  actionsTail = action;
}

void createLocationItemList(Location* location, char* items) {
  Item* foundItem;
  char i;

  for (i = 1; i <= items[0]; i++) {
    foundItem = findItemById(items[i]);
    createItemList(&(location->items), foundItem);
  }
}

void createLocationExitList(Location* location, char* exits) {
  Location* foundExit;
  char i;

  for (i = 1; i <= exits[0]; i++) {
    foundExit = findLocationById(exits[i]);
    if (foundExit == NULL) {
      foundExit = createLocation(exits[i], NULL, NULL, NULL, NULL);
      SGLIB_LIST_ADD_AFTER(Location, locationsTail, foundExit, next);
      locationsTail = foundExit;
    }
    createLocationList(&(location->exits), foundExit);
  }
}

void readLocation() {
  char i;
  char byteLen;
  char id;
  char* name;
  char* desc;
  char* exits;
  char* items;
  Location* location = NULL;
  ItemList* locationItems;

  id = readOneByte();
  location = findLocationById(id);

  byteLen = readOneByte();
  name = malloc(byteLen + 1);
  readNBytes(name, byteLen);
  name[byteLen] = 0;

  byteLen = readOneByte();
  desc = malloc(byteLen + 1);
  readNBytes(desc, byteLen);
  desc[byteLen] = 0;

  if (location == NULL) {
    location = createLocation(id, name, desc, NULL, NULL);
    SGLIB_LIST_ADD_AFTER(Location, locationsTail, location, next);
    if (locations == NULL) {
      locations = locationsTail;
    }
    locationsTail = location;
  } else {
    location->name = name;
    location->description = desc;
  }

  byteLen = readOneByte();
  exits = malloc(byteLen + 1);
  exits[0] = byteLen;
  readNBytes(exits + 1, byteLen);

  if (byteLen > 0) {
    createLocationExitList(location, exits);
  }

  byteLen = readOneByte();
  items = malloc(byteLen + 1);
  items[0] = byteLen;
  readNBytes(items + 1, byteLen);
  
  if (byteLen > 0) {
    createLocationItemList(location, items);
  }

}

void readObjects() {
  char type;

  type = readOneByte();
  while (!feof(fp)) {
    clrscr();
    if (type == OBJECT_ITEM) {
      printSplash("PARSING ITEM...");
      readItem();
    } else if (type == OBJECT_ACTION) {
      printSplash("PARSING ACTION...");
      readAction();
    } else if (type == OBJECT_LOCATION) {
      printSplash("PARSING LOCATION...");
      readLocation();
    }
    type = readOneByte();
  }
}

void parseConfigFile(char* filename) {
  locationsTail = NULL;
  actionsTail = NULL;

  fp = fopen(filename, "r");
  if (!fp) {
    con_error(ERR_FILE);
  }

  source = malloc((CONFIGFILEBUFFSIZE) * sizeof(char));
  if (source == NULL) {
    con_error(ERR_OUT_OF_MEMORY);
    return;
  }
  memset(source, 0, CONFIGFILEBUFFSIZE);

  printSplash("READING DATA FILE...");
  readObjects();
  
  //clean up
  free(source);
  fclose(fp);
}
