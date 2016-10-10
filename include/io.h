#include <conio.h>
#include <stdlib.h>

#ifndef YammadoreIO_h
#define YammadoreIO_h

#define COMMAND_SIZE 256
#define INPUT_WIDTH 30

#define printOutput(...) {\
  if (wherey() > 22) {\
    clrscr();\
    gotoxy(0, 0);\
  }\
  cprintf(##__VA_ARGS__);\
}

void acceptInput(char** input);

#endif
