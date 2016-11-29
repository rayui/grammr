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
size_t fp_bytes;

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
  char lastByte;
  lastByte = fgetc(fp);
  return lastByte;
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
}

void readLocation() {
  char byteLen;
  char id;
  char* name;
  char* desc;
  char* exits;
  char* items;
  Location* location;

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
  exits = malloc(byteLen + 1);
  exits[0] = byteLen;
  readNBytes(exits + 1, byteLen);

  byteLen = readOneByte();
  items = malloc(byteLen + 1);
  items[0] = byteLen;
  readNBytes(items + 1, byteLen);

  createLocation(id, name, desc, exits, items);
  SGLIB_LIST_ADD_AFTER(Location, locationsTail, location, next);
}

void readObjects() {
  char type;

  type = readOneByte();
  while (type != EOF) {
    if (type == OBJECT_ITEM) {
      readItem();
    } else if (type == OBJECT_ACTION) {
      readAction();
    } else if (type == OBJECT_LOCATION) {
      clrscr();
      readLocation();
      cgetc();
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
  
  fclose(fp);

  //clean up
  free(source);
}
