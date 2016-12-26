#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/io.h"
#include "../include/locations.h"
#include "../include/items.h"
#include "../include/utils.h"

extern const char str_command[];
const Coords clockPos = {0, 0};
const Coords promptPos = {0, 23};
const Coords locationPos = {8, 0};
const Coords textPos = {0, 8};
const Coords statusPos = {0, 2};
const Coords itemsPos = {0, 2};
const Coords exitsPos = {0, 5};

void printClock(long time) {
	char buf[9] = {0};
	decimalToTimeStr(buf, time);
	sprintf(buf, "%-8s\0", buf);
	textcolor(CLOCK_COLOUR);
	cputsxy(clockPos.x, clockPos.y, buf);
}

void printLocation(char* input) {
	textcolor(LOCATION_COLOUR);
	cputsxy(locationPos.x, locationPos.y, input);
}

void printSplash(char* input) {
	textcolor(SPLASH_COLOUR);
	cputsxy(statusPos.x, statusPos.y, input);
}

void printStatus(char* input) {
	textcolor(STATUS_COLOUR);
	cputsxy(statusPos.x, statusPos.y, input);
}

void printLocalExits(char* names) {
	gotoxy(exitsPos.x, exitsPos.y);
	textcolor(EXITS_COLOUR);
	if (strlen(names) > 0) {
		cprintf("Exits: %s", names);
	} else {
		cprintf("There are no exits");
	}
}

void printLocalItems(char* names) {
	gotoxy(itemsPos.x, itemsPos.y);
	textcolor(ITEMS_COLOUR);
	cprintf("You can see: %s", names);
}

void printOutput(char* output) {
	gotoxy(0, textPos.y);
	textcolor(TEXT_COLOUR);
	cprintf(output);
}

void printInstruction(long time, int equality, enum Instruction fn, char* location, char* subject, char* object, char *arg1, char *arg2) {
	char debug[128] = {0};
	char i = 6;

	clrscr();
	printClock(time);

	while (i--) {
		cclearxy(textPos.x + 6, textPos.y + i, 20);	
	}
	
	textcolor(DEBUG_COLOUR);
  sprintf(debug, "EQ: %d\r\nLN: %.24s\r\nSU: %.24s\r\nOB: %.24s\r\n\r\nFN: %03d\r\nA1: %.24s\r\nA2: %.24s", equality, location, subject ? subject : "NULL", object ? object : "NULL", fn, arg1, arg2);
  cputsxy(textPos.x, textPos.y + 2, debug);
}

void printPrompt(char* input) {
		int offset = strlen(input) - INPUT_WIDTH;
		char* output = malloc(sizeof(char) * INPUT_WIDTH);
		memset(output, 0, INPUT_WIDTH);

		textcolor(PROMPT_COLOUR);
		cputsxy(promptPos.x, promptPos.y, output);
		cputsxy(promptPos.x, promptPos.y, str_command);
		free(output);

		if (offset < 0) {
			offset = 0;
		}
		output = input + offset;

		cputsxy(promptPos.x + 2, promptPos.y, output);
		gotoxy(promptPos.x + 2 + strlen(output), promptPos.y);
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



