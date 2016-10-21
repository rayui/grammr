#define SGLIB_LIST_ADD_AFTER(type, place, elem, next) {\
  if ((place) == NULL) {\
    (place) = (elem);\
    (place)->next = NULL;\
  } else {\
    (elem)->next = (place)->next;\
    (place)->next = (elem);\
  }\
}

void *realloc_it(void *ptrmem, size_t size);
void toC64Case(char* str);
void toLowerCase(char* str);
char toLowerCaseContains(char *str1, char* str2);
char toLowerCaseCompare(char *str1, char* str2);
char *replace_str(char *str, char *orig, char *rep);
int numItemsInCharArray(size_t arraySize);
int isStringNumeric(char* val);
int isStringAlpha(char *s);
char strComp(char *str1, char* str2);
void decimalToTimeStr(char* timeStr, long time);