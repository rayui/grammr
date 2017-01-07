#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <string.h>
#include "../include/main.h"
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/utils.h"
#include "../include/lexer.h"
#include "../include/instruction.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/items.h"
#include "../include/config_parser.h"

Token* tokenHead = NULL;
ErrorList* errorList = NULL;
Actions* actions = NULL;
Item* items = NULL;
Location* currentLocation = NULL;
Location* locations = NULL;
ItemList* inventory = NULL;
long CLOCK = 0;
enum RunState RUNSTATE = SE_OK;

void drawSplash() {
  clrscr();
  bordercolor(BG_COLOR);
  bgcolor(BG_COLOR);
  cursor(true);

  printStatus("LOADING GAME...");
}

void drawHUD() {
  char buf[256] = {0};

  printClock(CLOCK);
  printLocation(currentLocation->name);

  getAllLocationNames(currentLocation->exits, buf);
  printLocalExits(buf);

  buf[0] = 0;
  getAllItemNames(currentLocation->items, buf);
  printLocalItems(buf);
}

int main() {
  char* input = malloc(DEFAULTSTRINGSIZE * sizeof(char));
  char* output = malloc(MAXOUTPUTSIZE * sizeof(char));

  drawSplash();

  parseConfigFile("data.pet");
  clrscr();

  if (RUNSTATE == SE_OK) {
    currentLocation = locations;

    while(RUNSTATE != SE_TERMINAL) {
      memset(input, 0, DEFAULTSTRINGSIZE);
      output[0] = 0;
      tokenHead = NULL;
      RUNSTATE = SE_OK;

      drawHUD();
      acceptInput(&input);
      clrscr();

      lex(&tokenHead, input);

      if(RUNSTATE == SE_OK) {
        parse(&tokenHead, output);
      }

      free_tokens(&tokenHead);
      clrscr();

      if (RUNSTATE != SE_OK) {
        print_errors(input, output);
      }

      printOutput(output);
      free_errors();
    }

    free(input);
    free(output);

    return EXIT_SUCCESS;
  }
}
