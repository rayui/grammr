#include <conio.h>
#include <stdlib.h>

#ifndef IO
#define IO

#define INPUT_WIDTH 30
#define OUTPUT_WIDTH 40

#define BG_COLOUR 0         //BLACK
#define SPLASH_COLOUR 13    //LIGHT GREEN
#define CLOCK_COLOUR 15     //LIGHT GREY
#define PROMPT_COLOUR 13    //LIGHT GREEN
#define LOCATION_COLOUR 15  //LIGHT GREY
#define TEXT_COLOUR 1       //WHITE
#define STATUS_COLOUR 10    //LIGHT RED
#define SPINNER_COLOUR 1    //WHITE
#define ITEMS_COLOUR 14     //LIGHT BLUE
#define EXITS_COLOUR 14     //LIGHT BLUE
#define DEBUG_COLOUR 5      //GREEN

typedef struct Coords {
  char x;
  char y;
} Coords;

void printSpinner(char spin);
void printClock(long time);
void printLocation(char* input);
void printLocalItems(char* names);
void printLocalExits(char* names);
void acceptInput(char** input);
void printPrompt(char* input);
void printStatus(char* input);
void printOutput(char* output);
void printInstruction(int equality, enum Instruction fn, char* location, char* subject, char* object, char *arg1, char *arg2);

#endif
