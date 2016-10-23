#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/instruction.h"
#include "../include/yammadore.h"
#include "../include/utils.h"
#include "../include/items.h"

extern Item* items;
extern ItemList* inventory;

Item* createItem(char* name, char* description) {
  char i = 0;
  Item* item = malloc(sizeof(Item));

  item->name = malloc((strlen(name) + 1));
  strcpy(item->name, name);

  item->description = malloc((strlen(description) + 1));
  strcpy(item->description, description);

  item->actions = NULL;

  SGLIB_LIST_ADD(Item, items, item, next);

  return item;
}

ItemList* createItemList(ItemList** items, Item* item) {
  ItemList* itemList = malloc(sizeof(ItemList));

  itemList->item = item;

  SGLIB_LIST_ADD(ItemList, *items, itemList, next);

  return itemList;
}

void deleteItemList(ItemList** items, Item* item) {
  SGLIB_LIST_MAP_ON_ELEMENTS(ItemList, *items, listItem, next, {
    if (listItem->item == item) {
      SGLIB_LIST_DELETE(ItemList, *items, listItem, next);
      free(listItem);
    }
  });
}

Item* findItemByName(char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(Item, items, item, next, {
    if (toLowerCaseCompare(name, item->name)) {
      return item;
    }
  });

  return NULL;
}

Item* findItemInList(ItemList* list, char* name) {
  SGLIB_LIST_MAP_ON_ELEMENTS(ItemList, list, listItem, next, {
    if (toLowerCaseCompare((char *)listItem->item->name, name)) {
      return listItem->item;
    }
  });
  return NULL;
}

char inventoryHasItem(char* name) {
  char foundItem = 0;

  SGLIB_LIST_MAP_ON_ELEMENTS(ItemList, inventory, listItem, next, {
    if (toLowerCaseCompare((char *)listItem->item->name, name)) {
      foundItem = 1;
    }
  });

  return foundItem;
}

void getAllItemNames(ItemList* list, char* itemNames) {
  Item* item;

  SGLIB_LIST_MAP_ON_ELEMENTS(ItemList, list, itemList, next, {
    item = itemList->item;
    strcat(itemNames, item->name);
    if (itemList->next != NULL) strcat(itemNames, ", ");
  });
}
