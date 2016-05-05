#ifndef LOCATIONS
#define LOCATIONS

#define MAX_LOCATION_NAME_LENGTH 64
#define MAX_LOCATION_DESCRIPTION_LENGTH 512

typedef struct Location {
  char name[MAX_LOCATION_NAME_LENGTH];
  char description[MAX_LOCATION_DESCRIPTION_LENGTH];
  struct LocationList* exits;
  struct ItemList* items;
  struct Location* next;
} Location;

typedef struct LocationList {
  struct Location* location;
  struct LocationList* next;
} LocationList;

#endif

struct Location* createLocation(char* name, char* description, struct LocationList* exits, struct ItemList* items);
struct LocationList* createLocationList(LocationList** locations, Location* location);
struct Location* findLocationByName(char* name);
void getAllLocationNames(LocationList* list, char* locationNames);
struct Location* moveToLocation(char* name);
int locationHasExit(char* locationName, char* exitName);
void locationDescription(char* locationName);
