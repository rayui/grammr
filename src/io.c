#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/io.h"

extern const char str_command[];

void printPrompt(char* input) {
		int offset = strlen(input) - INPUT_WIDTH;
		char* output = malloc(sizeof(char) * INPUT_WIDTH);
		memset(output, 0, INPUT_WIDTH);
		cputsxy(0, 24, output);
		cputsxy(0, 24, str_command);
		free(output);

		if (offset < 0) {
			offset = 0;
		}
		output = input + offset;

		cputsxy(2, 24, output);
		gotoxy(2 + strlen(output), 24);
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



