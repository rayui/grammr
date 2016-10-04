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
#include "../include/items.h"
#include "../include/config_parser.h"

Token* tokenList = NULL;
InstructionList* instructionList = NULL;
ErrorList* errorList = NULL;
Actions* actions = NULL;
Item* items = NULL;
Location* currentLocation;
Location* locations = NULL;
ItemList* inventory = NULL;

void ifError(enum RunState err, char* input, char* output) {
  if (err != SE_OK) {
    sprintf(output, "You wrote: %s\r\n\r\n", input);
    print_errors(err, output);
  }
}

int main() {
  int i = 0;
  enum RunState err = SE_OK;
  char* input = malloc(COMMAND_SIZE * sizeof(char));
  char* output = malloc(MAXOUTPUTSIZE * sizeof(char));

  clrscr();
  bordercolor(BG_COLOR);
  bgcolor(BG_COLOR);
  textcolor(TEXT_COLOR);
  cursor(true);

  PRINT("LOADING GAME...");

  err = parseConfigFile("data.pet");

  if (err == SE_OK) {
    currentLocation = locations;

    while(err != SE_TERMINAL) {
      err = SE_OK;

      memset(input, 0, COMMAND_SIZE);
      memset(output, 0, MAXOUTPUTSIZE);
      acceptInput(&input);

      clrscr();

      PRINT(".");
      err = lex(input);
      ifError(err, input, output);

      if(err == SE_OK) {
        PRINT(".");
        err = parse();
        ifError(err, input, output);
        if(err == SE_OK) {
          PRINT(".");
          err = interpret(output);
          ifError(err, input, output);
        }
        free_parser();
      }

      free_lexer();

      PRINT("%s\r\n", output);

      free_errors();
    }
  }

  ifError(err, input, output);
  free_errors();

  free(input);
  free(output);

	return EXIT_SUCCESS;
}
