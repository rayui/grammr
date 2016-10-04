#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/io.h"

extern const char str_command[];

void printPrompt(char* input) {
		cputsxy(0, 24, "                              ");
		cputsxy(0, 24, str_command);
		cputsxy(2, 24, input);
		gotoxy(2 + strlen(input), 24);
}

void acceptInput(char** input) {
	char count = 0;
	char charIn = 0;

	printPrompt("\0");

	while (charIn != 13 && count < COMMAND_SIZE) {

		charIn = cgetc();

		//20 is delete/backspace in PETSCII
		if (charIn == 20 && count > 0) {
			(*input)[count -  1] = 0;
			count--;
		} else if  (charIn == 32 ||
								charIn >= 65 && charIn <= 90 ||
								charIn >= 97 && charIn <= 122) {
			(*input)[count] = charIn;
			count++;
		} else if (charIn == 13) {
			(*input)[count] = '\0';
			count++;
		}

		printPrompt(*input);
	}
}



