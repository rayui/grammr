#ifndef ACTIONS
#define ACTIONS

#define MAX_ACTION_SIZE 64
#define MAX_ACTIONS 5

typedef struct Actions {
  char id;
  char name[MAX_ACTION_SIZE];
  char args;
  char isDefault;
  char* instructions;
  struct Actions* next;
} Actions;

Actions* createAction(char id, char* name, char* instructions, char isDefault);
Actions* findActionById(Actions* actions, char id);
Actions* findActionByName(Actions* actions, char* name);
Actions* findActionByNameAndItem(Actions* actions, struct Item* item, char* name);
Actions* findDefaultActionByName(Actions* actions, char* name);

#endif