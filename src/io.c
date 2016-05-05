#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/io.h"

extern const char str_command[];

int getAndEchoKeyboardInput(void) {
  int c;
	PUT(c = (int)cgetc());
	return c;
}

void acceptInput(char** input) {
	int count = 0;
	int charIn;

	PRINT("%s ", str_command);

	while ((charIn = getAndEchoKeyboardInput()) != '\n' && count < COMMAND_SIZE) {
		(*input)[count] = charIn;
		count++;
	}

	PRINT("\r\n");

	(*input)[count] = 0;

}



