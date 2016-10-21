#include <conio.h>
#include <stdlib.h>

#ifndef IO
#define IO

#define COMMAND_SIZE 256
#define INPUT_WIDTH 30

typedef struct Coords {
  char x;
  char y;
} Coords;

void printClock(void);
void printLocation(char* input);
void printLocalItems(struct ItemList* items);
void printLocalExits(struct LocationList* exits);
void acceptInput(char** input);
void printPrompt(char* input);
void printStatus(char* input);
void printOutput(char* output);
void printInstruction(int equality, enum Instruction fn, char *arg1, char *arg2);

#endif
