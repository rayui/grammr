#define MAXERRSIZE 512

enum RunState {
  SE_OK,
  SE_WARNING,
  SE_TERMINAL,
  SE_QUIT
};

enum ErrorType {
  LEX_UNRECOGNISED_TOKEN,
  CC_ERR_END_OF_COMMAND_EXPECTED,
  CC_ERR_COMMAND_EXPECTED,
  CC_WORD_EXPECTED,
  CC_DIRECTION_EXPECTED,
  CC_ITEM_EXPECTED,
  CC_PREPOSITION_EXPECTED,
  CC_NO_MEMORY,
  CC_VERB_EXPECTED,
  CC_CONJUNCTION_EXPECTED,
  CC_NO_SUCH_ACTION,
  CC_ITEM_UNAVAILABLE,
  CC_INCORRECT_NUMBER_OF_ARGS,
  CC_QUIT,
  CON_VERB_EXPECTED,
  CON_FILE_ERROR,
  CON_UNEXPECTED_EOF,
  CON_NO_MEMORY,
  CON_UNEXPECTED_TOKEN,
  CON_ARRAY_EXPECTED,
  CON_OBJECT_EXPECTED,
  CON_NUMER_EXPECTED,
  CON_TOO_MANY_ACTIONS,
  CON_NAME_EXPECTED,
  CON_DESCRIPTION_EXPECTED,
  CON_ITEM_NOT_FOUND,
  CON_LOCATION_NOT_FOUND,
  CON_ACTION_OBJECT_EXPECTED,
  CON_ACTION_TYPE_EXPECTED,
  CON_ACTION_COMMAND_EXPECTED,
  CON_ILLEGAL_INSTRUCTION,
  CON_OVERSIZE_INSTRUCTION,
};

typedef struct ErrorList {
  enum ErrorType error;
  char val[MAXERRSIZE];
  struct ErrorList* next;
} ErrorList;

void print_error(char* output, enum ErrorType errorCode, char* val);
void print_errors(enum RunState, char* output);
void free_errors(void);
void create_error(enum ErrorType error, char* val);
