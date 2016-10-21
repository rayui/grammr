#ifndef ITEMS
#define ITEMS

#define MAX_ITEM_NAME_LENGTH 40
#define MAX_ITEM_DESCRIPTION_LENGTH 512
#define MAXINSTRUCTIONS 5

typedef struct Item {
  char *name;
  char *description;
  int actions[MAXINSTRUCTIONS];
  struct Item *next;
} Item;

typedef struct ItemList {
  struct Item *item;
  struct ItemList *next;
} ItemList;

#endif

Item* createItem(char* name, char* description);
ItemList* createItemList(ItemList** items, Item* item);
void deleteItemList(ItemList** items, Item* item);
struct Item* findItemByName(char* name);
Item* findItemInList(ItemList* list, char* name);
char inventoryHasItem(char* name);
void getAllItemNames(ItemList* list, char* itemNames);
