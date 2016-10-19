#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <string.h>
#include "../include/sglib.h"
#include "../include/yammadore.h"
#include "../include/io.h"
#include "../include/error.h"
#include "../include/lexer.h"
#include "../include/instruction.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/items.h"
#include "../include/config_parser.h"

Token* tokenHead = NULL;
InstructionList* instructions = NULL;
ErrorList* errorList = NULL;
Actions* actions = NULL;
Item* items = NULL;
Location* currentLocation;
Location* locations = NULL;
ItemList* inventory = NULL;
enum RunState RUNSTATE = SE_OK;

int main() {
  int i = 0;
  char* input = malloc(COMMAND_SIZE * sizeof(char));
  char* output = malloc(MAXOUTPUTSIZE * sizeof(char));

  clrscr();
  bordercolor(BG_COLOR);
  bgcolor(BG_COLOR);
  textcolor(TEXT_COLOR);
  cursor(true);

  printOutput("LOADING GAME...");

  parseConfigFile("data.pet");

  if (RUNSTATE == SE_OK) {
    currentLocation = locations;

    while(RUNSTATE != SE_TERMINAL) {
      RUNSTATE = SE_OK;

      memset(input, 0, COMMAND_SIZE);
      memset(output, 0, MAXOUTPUTSIZE);
      acceptInput(&input);

      tokenHead = NULL;
      instructions = NULL;

      clrscr();

      printOutput(".");
      lex(&tokenHead, input);

      if(RUNSTATE == SE_OK) {
        printOutput(".");
        parse(&tokenHead, &instructions);
        if(RUNSTATE == SE_OK) {
          printOutput(".");
          interpret(&instructions, output);
        }
        free_instructions(instructions);
      }

      free_tokens(&tokenHead);
      //clrscr();

      if (RUNSTATE != SE_OK) {
        print_errors(input, output);
      }
      printOutput("%s\r\n", output);

      free_errors();
    }
  }

  print_errors("", output);
  free_errors();

  free(input);
  free(output);

	return EXIT_SUCCESS;
}
