#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/error.h"
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/io.h"
#include "../include/locations.h"
#include "../include/items.h"

extern Location *locations;
extern Location *currentLocation;

Location* createLocation(char* name, char* description, LocationList* exits, ItemList* items)
{
  Location* location = malloc(sizeof(Location));
  if (location == NULL) {
    create_error(SE_TERMINAL, ERR_OUT_OF_MEMORY, name);
    return NULL;
  }

  location->name = malloc(strlen(name) + 1);
  location->description = malloc(strlen(description) + 1);

  strcpy(location->name, name);
  strcpy(location->description, description);

  location->exits = exits;
  location->items = items;

  return location;
}

LocationList* createLocationList(LocationList** locations, Location* location) {
  LocationList* locationList = malloc(sizeof(LocationList));

  locationList->location = location;

  SGLIB_LIST_ADD(LocationList, *locations, locationList, next);

  return locationList;
}

Location* findLocationByName(char* name) {
  Location* foundLocation = NULL;

  SGLIB_LIST_MAP_ON_ELEMENTS(Location, locations, location, next, {
    if (toLowerCaseCompare(name, location->name)) {
      foundLocation = location;
    }
  });

  return foundLocation;
}

int locationHasExit(char* locationName, char* exitName) {
  char foundLocation = 0;
  Location* location = findLocationByName(locationName);

  SGLIB_LIST_MAP_ON_ELEMENTS(LocationList, location->exits, locationExit, next, {
    if (toLowerCaseCompare(locationExit->location->name, exitName)) {
      foundLocation = 1;
    }
  });

  return foundLocation;
}

void getAllLocationNames(LocationList* list, char* locationNames) {
  Location* location;

  SGLIB_LIST_MAP_ON_ELEMENTS(LocationList, list, listLocation, next, {
    location = listLocation->location;
    strcat(locationNames, location->name);
    if (listLocation->next != NULL) strcat(locationNames, ", ");
  });
}

void locationDescription(char* locationName) {
  Location* location = findLocationByName(locationName);
  strcpy(locationName, location->description);
}
