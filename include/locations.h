#ifndef LOCATIONS
#define LOCATIONS

typedef struct Location {
  char id;
  char* name;
  char* description;
  struct LocationList* exits;
  struct ItemList* items;
  struct Location* next;
} Location;

typedef struct LocationList {
  struct Location* location;
  struct LocationList* next;
} LocationList;

#endif

struct Location* createLocation(char id, char* name, char* description, struct LocationList* exits, struct ItemList* items);
struct LocationList* createLocationList(LocationList** locations, Location* location);
struct Location* findLocationByName(char* name);
struct Location* findLocationById(char id);
void getAllLocationNames(LocationList* list, char* locationNames);
struct Location* moveToLocation(char* name);
int locationHasExit(char* locationName, char* exitName);
void locationDescription(char* locationName);
