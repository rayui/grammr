#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/main.h"
#include "../include/utils.h"
#include "../include/lexer.h"

//normalise string to high char c64 symbols
//make sure to copy original string to new memory location first if required
void toC64Case(char* str) {
  int i = 0;

  while (str[i] != '\0') {
    if (str[i] >= 97 && str[i] <= 122) {
      str[i] = str[i] + 96;
    }
    i++;
  }
}

//lower cases string in place
//make sure to copy original string to new memory location first if required
void toLowerCase(char* str) {
  int i = 0;

  if (str == NULL) {
    return;
  }

  while (str[i] != '\0') {
    if (str[i] >= 97 && str[i] <= 122) {
      str[i] = str[i] - 32;
    } else if (str[i] >= 193 && str[i] <= 218) {
      str[i] = str[i] - 128;
    }
    i++;
  }
}

char toLowerCaseContains(char *str1, char* str2) {
  char compare = 0;
  char* str1Lwr;
  char* str2Lwr;

  if (str1 == NULL || str2 == NULL)
    return compare;

  str1Lwr = malloc(strlen(str1));
  str2Lwr = malloc(strlen(str2));

  strcpy(str1Lwr, str1);
  strcpy(str2Lwr, str2);

  toLowerCase(str1Lwr);
  toLowerCase(str2Lwr);

  if (strstr(str1Lwr, str2Lwr) != NULL) {
    compare = 1;
  }

  free(str1Lwr);
  free(str2Lwr);

  return compare;
}

char strComp(char *str1, char* str2) {
  if (strcmp(str1, str2) == 0) {
    return 1;
  }
  return 0;
}

char toLowerCaseCompare(char *str1, char* str2) {
  char compare = 0;
  char* str1Lwr;
  char* str2Lwr;

  if (str1 == NULL || str2 == NULL)
    return compare;

  str1Lwr = malloc(strlen(str1));  
  str2Lwr = malloc(strlen(str2));

  strcpy(str1Lwr, str1);
  strcpy(str2Lwr, str2);

  toLowerCase(str1Lwr);
  toLowerCase(str2Lwr);

  compare = strComp(str1Lwr, str2Lwr);

  free(str1Lwr);
  free(str2Lwr);

  return compare;
}


/**
 * The replace function
 *
 * Searches all of the occurrences using recursion
 * and replaces with the given string
 * @param char * o_string The original string
 * @param char * s_string The string to search for
 * @param char * r_string The replace string
 * @return void The o_string passed is modified
 */
void replace_str(char * o_string, char * s_string, char * r_string) {
  //a buffer variable to do all replace things
  char buffer[DEFAULTSTRINGSIZE];
  //to store the pointer returned from strstr
  char * ch;

  //first exit condition
  if(!(ch = strstr(o_string, s_string)))
          return;

  //copy all the content to buffer before the first occurrence of the search string
  strncpy(buffer, o_string, ch-o_string);

  //prepare the buffer for appending by adding a null to the end of it
  buffer[ch-o_string] = 0;

  //append using sprintf function
  sprintf(buffer+(ch - o_string), "%s%s", r_string, ch + strlen(s_string));

  //empty o_string for copying
  o_string[0] = 0;
  strcpy(o_string, buffer);
  //pass recursively to replace other occurrences
  replace_str(o_string, s_string, r_string);
 }

int numItemsInCharArray(size_t arraySize) {
  size_t numItems = arraySize / sizeof (const char*);
  return (int)numItems;
}

int isStringNumeric(char* val) {
  int i = 0;

  for (i = 0; val[i] != '\0'; i++){
    if (!isdigit(val[i])) {
      return 0;
    }
  }

  return 1;
}

int isStringAlpha(char *s) {
  while(isalnum(*s))
    ++s;
  return *s == 0;
}

void decimalToTimeStr(char* timeStr, long time) {
  long slowTime = time >> 2;
  char days = 0;
  char hours = 0;
  char minutes = 0;

  minutes = slowTime % 60;
  hours = ((slowTime - minutes) / 60) % 24;

  sprintf(timeStr, "%02d:%02d", hours, minutes);
}

char getJustifyLen(char* string, char width) {
  char i = width;
  char* hasBreak = strchr(string, 13);

  if (strlen(string) < width) {
    return strlen(string);
  }

  if (hasBreak) {
    if (hasBreak - string < width)
      return hasBreak - string;
  }

  while (i--) {
    if (string[i] == 32) {
      return i;
    }
  }

  return strlen(string);
}

void justifyString(char* input, char* output, char width) {
  char* offset = input;
  char len = getJustifyLen(offset, width);

  output[0] = '\0';

  while ( offset - input < strlen(input) ) {
    sprintf(output, "%s%.*s\r\n", output, len, offset);
    offset += len + 1;  //+1 removes the space also
    len = getJustifyLen(offset, width);
  }
}