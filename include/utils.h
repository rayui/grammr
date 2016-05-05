void *realloc_it(void *ptrmem, size_t size);
void toC64Case(char* str);
void toLowerCase(char* str);
char toLowerCaseContains(char *str1, char* str2);
char toLowerCaseCompare(char *str1, char* str2);
char *replace_str(char *str, char *orig, char *rep);
int numItemsInCharArray(size_t arraySize);
int isStringNumeric(char* val);
int isStringAlpha(char *s);

//list is the item after which to insert elem
//also attaches next element
#define SGLIB_LIST_APPEND(type, list, elem, next) {\
  type *_list_;\
  _list_ = (list);\
  while (_list_->next!=NULL) {\
    _list_=_list_->next;\
  }\
  _list_->next=elem;\
}

