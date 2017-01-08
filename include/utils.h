#define SGLIB_LIST_ADD_AFTER(type, place, elem, next) {\
  if ((place) == NULL) {\
    (place) = (elem);\
    (place)->next = NULL;\
  } else {\
    (elem)->next = (place)->next;\
    (place)->next = (elem);\
  }\
}

void toC64Case(char* str);
void toLowerCase(char* str);
char toLowerCaseContains(char *str1, char* str2);
char toLowerCaseCompare(char *str1, char* str2);
void replace_str(char * o_string, char * s_string, char * r_string);
int numItemsInCharArray(size_t arraySize);
int isStringNumeric(char* val);
int isStringAlpha(char *s);
char strComp(char *str1, char* str2);
void decimalToTimeStr(char* timeStr, long time);
void justifyString(char* input, char* output, char width);