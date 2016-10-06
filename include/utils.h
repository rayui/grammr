#define SGLIB_LIST_ADD_POST(type, list, elem, next) {\
  if ((list) == NULL) {\
    (list) = (elem);\
    (list)->next = NULL;\
  } else {\
    (list)->next = (elem);\
    (elem)->next = NULL;\
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