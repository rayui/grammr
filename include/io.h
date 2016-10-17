#include <conio.h>
#include <stdlib.h>

#ifndef IO
#define IO

#define COMMAND_SIZE 256
#define INPUT_WIDTH 30

#define printOutput(...) {\
  if (wherey() > 22) {\
    cgetc();\
    clrscr();\
    gotoxy(0, 0);\
  }\
  cprintf(##__VA_ARGS__);\
}

void acceptInput(char** input);

#endif
