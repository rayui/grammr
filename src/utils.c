#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/utils.h"
#include "../include/lexer.h"

/* Function realloc_it() is a wrapper function for standart realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standart realloc() instead.
 */

//stolen from https://github.com/zserge/jsmn/blob/master/example/jsondump.c

char* getNextCharBeforeLimit(char* haystack, char needle, int limit) {
  char buf[64] = {'\0'};

  strncpy(buf, haystack, limit);

  return strrchr(buf, needle);
}

void *realloc_it(void *ptrmem, size_t size) {
  void *p = realloc(ptrmem, size);
  if (!p)  {
    free (ptrmem);
  }
  return p;
}

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
  char* str1Lwr = malloc(strlen(str1));
  char* str2Lwr = malloc(strlen(str2));

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
  char* str1Lwr = malloc(strlen(str1));
  char* str2Lwr = malloc(strlen(str2));

  strcpy(str1Lwr, str1);
  strcpy(str2Lwr, str2);

  toLowerCase(str1Lwr);
  toLowerCase(str2Lwr);

  if (strcmp(str1Lwr, str2Lwr) == 0) {
    compare = 1;
  }

  free(str1Lwr);
  free(str2Lwr);

  return compare;
}

char *replace_str(char *str, char *orig, char *rep) {
  static char buffer[128];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
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