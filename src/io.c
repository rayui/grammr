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
extern long CLOCK;
const Coords clockPos = {0, 0};
const Coords promptPos = {0, 23};
const Coords locationPos = {8, 0};
const Coords textPos = {0, 8};
const Coords statusPos = {0, 2};
const Coords itemsPos = {0, 2};
const Coords exitsPos = {0, 5};

void printClock(void) {
	char timeStr[8];
	decimalToTimeStr(timeStr, CLOCK);
	sprintf(timeStr, "%s", timeStr);
	cputsxy(clockPos.x, clockPos.y, timeStr);
}

void printLocation(char* input) {
	cputsxy(locationPos.x, locationPos.y, input);
}

void printStatus(char* input) {
	cputsxy(statusPos.x, statusPos.y, input);
}

void printLocalExits(struct LocationList* exits) {
	char names[256] = {0};
	getAllLocationNames(exits, names);
	gotoxy(exitsPos.x, exitsPos.y);
	if (strlen(names) > 0) {
		cprintf("Exits: %s", names);
	} else {
		cprintf("There are no exits");
	}
}

void printLocalItems(struct ItemList* items) {
	char names[256] = {0};
	getAllItemNames(items, names);
	gotoxy(itemsPos.x, itemsPos.y);

	cprintf("You can see: %s", names);
}

void printOutput(char* output) {
	gotoxy(0, textPos.y);
	cprintf(output);
}

void printInstruction(int equality, enum Instruction fn, char *arg1, char *arg2) {
	char debug[64];
	char i = 6;
	while (i--) {
		cclearxy(textPos.x + 6, textPos.y + i, 20);	
	}
  sprintf(debug, "TIME: %lu\r\n", CLOCK);
  cputsxy(textPos.x, textPos.y, debug);
  sprintf(debug, "EQ:   %d\r\nFN: %02X\r\nA1: %.10s\r\nA2: %.10s", equality, fn, arg1, arg2);
  cputsxy(textPos.x, textPos.y + 2, debug);
}

void printPrompt(char* input) {
		int offset = strlen(input) - INPUT_WIDTH;
		char* output = malloc(sizeof(char) * INPUT_WIDTH);
		memset(output, 0, INPUT_WIDTH);
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



