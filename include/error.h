#define MAXERRSIZE 128

enum RunState {
  SE_OK,
  SE_CONFIG,
  SE_PARSER,
  SE_INTRPRT,
  SE_TERMINAL,
  SE_QUIT
};

enum ErrorType {
  ERR_UNRECOGNISED_TOKEN,
  ERR_END_OF_COMMAND_EXPECTED,
  ERR_COMMAND_EXPECTED,
  ERR_ITEM_EXPECTED,
  ERR_PREPOSITION_EXPECTED,
  ERR_OUT_OF_MEMORY,
  ERR_NO_SUCH_ACTION,
  ERR_INVALID_INSTRUCTION,
  ERR_UNKNOWN_INSTRUCTION,
  ERR_ITEM_NOT_FOUND,
  ERR_LOCATION_NOT_FOUND,
  ERR_QUIT,
  ERR_FILE,
  ERR_JSON_UNEXPECTED_TOKEN,
  ERR_JSON_ARRAY_EXPECTED,
  ERR_JSON_OBJECT_EXPECTED,
  ERR_JSON_NUMBER_EXPECTED,
  ERR_CONFIG_TOO_MANY_ACTIONS,
  ERR_CONFIG_NAME_EXPECTED,
  ERR_CONFIG_DESCRIPTION_EXPECTED,
  ERR_CONFIG_OVERSIZE_INSTRUCTION,
  ERR_CONFIG_BAD_JSON
};

typedef struct ErrorList {
  enum ErrorType error;
  char val[MAXERRSIZE];
  struct ErrorList* next;
} ErrorList;

void print_error(char* output, enum ErrorType errorCode, char* val);
void print_errors(char* input, char* output);
void free_errors(void);
void create_error(enum RunState state, enum ErrorType error, char* val);
