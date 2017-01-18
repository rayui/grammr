#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/main.h"
#include "../include/io.h"
#include "../include/locations.h"
#include "../include/items.h"
#include "../include/utils.h"

extern const char str_command[];
const Coords clockPos = {0, 0};
const Coords promptPos = {0, 23};
const Coords locationPos = {8, 0};
const Coords textPos = {0, 8};
const Coords statusPos = {0, 0};
const Coords itemsPos = {0, 2};
const Coords exitsPos = {0, 5};
const Coords spinnerPos = {39, 0};
const char spinner[4] = {0xBE, 0xBC, 0xAC, 0xBB};

void printSpinner(char spin) {
	textcolor(SPINNER_COLOUR);
	cputcxy(spinnerPos.x, spinnerPos.y, spinner[spin % 4]);
}

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

void printStatus(char* input) {
	char buf[40] = {0};
	sprintf(buf, "%-*s", OUTPUT_WIDTH, input);
	textcolor(SPLASH_COLOUR);
	cputsxy(statusPos.x, statusPos.y, buf);
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
	char* tmpStr = malloc(MAXOUTPUTSIZE * sizeof(char));
	justifyString(output, tmpStr, OUTPUT_WIDTH);

	textcolor(TEXT_COLOUR);
	gotoxy(textPos.x, textPos.y);
	cprintf(tmpStr);

	free(tmpStr);
}

void printInstruction(int equality, enum InstructionType fn, char* location, char* subject, char* object, char *arg1, char *arg2) {
	char debug[DEFAULTSTRINGSIZE] = {0};
	char i = 8;

	while (i--) {
		cclearxy(textPos.x, textPos.y + i, OUTPUT_WIDTH - textPos.x);
	}
	
	textcolor(DEBUG_COLOUR);
  sprintf(debug, "EQ: %d\r\nLN: %.24s\r\nSU: %.24s\r\nOB: %.24s\r\n\r\nFN: %03d\r\nA1: %.24s\r\nA2: %.24s", equality, location, subject ? subject : "NULL", object ? object : "NULL", fn, arg1 ? arg1 : "NULL", arg2 ? arg2 : "NULL");
  cputsxy(textPos.x, textPos.y, debug);
}

void printPrompt(char* input) {
		int offset = strlen(input) - INPUT_WIDTH;
		char* output = NULL;

		textcolor(PROMPT_COLOUR);
		cputsxy(promptPos.x, promptPos.y, str_command);

		if (offset < 0)
			offset = 0;

		output = input + offset;

		cclearxy(promptPos.x + 2 + strlen(input), promptPos.y, 1);
		cputsxy(promptPos.x + 2, promptPos.y, output);
}

void acceptInput(char** input) {
	char count = 0;
	char charIn = 0;

	printPrompt("");

	while (charIn != 13) {
		charIn = cgetc();
		//20 is delete/backspace in PETSCII
		if (charIn == 20 && count > 0) {
			count--;
			(*input)[count] = '\0';
		} else if (
			(charIn == 32 ||
			charIn >= 65 && charIn <= 90 ||
			charIn >= 97 && charIn <= 122) &&
			count < DEFAULTSTRINGSIZE - 1
		) {
			(*input)[count] = charIn;
			count++;
		} else if (charIn == 13) {
			(*input)[count] = '\0';
			count++;
		}

		printPrompt(*input);
	}
}



