#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/utils.h"
#include "../include/io.h"
#include "../include/error.h"

extern const char str_syntax_error_end_of_command_expected[];
extern const char str_syntax_error_command_expected[];
extern const char str_syntax_error_item_expected[];
extern const char str_syntax_error_preposition_expected[];
extern const char str_syntax_error_unrecognised_token[];
extern const char str_syntax_error_no_such_action[];
extern const char str_parse_error_item_not_found[];
extern const char str_terminal_system_error[];
extern const char str_system_error_template[];
extern const char str_system_error_no_memory[];
extern const char str_parse_error_item_not_in_inventory[];
extern const char  str_parse_error_item_in_inventory[];

extern ErrorList* errorList;
extern enum RunState RUNSTATE;

ErrorList* lastError = NULL;

void print_error(char* output, enum ErrorType errorCode, char* val) {
  char tmpOutput[MAXERRSIZE];

  switch(errorCode) {
    case ERR_END_OF_COMMAND_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_end_of_command_expected);
      break;
    case ERR_COMMAND_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_command_expected);
      break;
    case ERR_ITEM_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_item_expected, val);
      break;
    case ERR_PREPOSITION_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_preposition_expected);
      break;
    case ERR_UNRECOGNISED_TOKEN:
      sprintf(tmpOutput, str_syntax_error_unrecognised_token);
      break;
    case ERR_NO_SUCH_ACTION:
      sprintf(tmpOutput, str_syntax_error_no_such_action, val);
      break;
    case ERR_OUT_OF_MEMORY:
      sprintf(tmpOutput, str_system_error_no_memory, val);
      break;
    case ERR_ITEM_NOT_FOUND:
      sprintf(tmpOutput, str_parse_error_item_not_found, val);
      break;
    case ERR_ITEM_NOT_IN_INVENTORY:
      sprintf(tmpOutput, str_parse_error_item_not_in_inventory, val);
      break;
    case ERR_ITEM_IN_INVENTORY:
      sprintf(tmpOutput, str_parse_error_item_in_inventory, val);
      break;
    default:
      sprintf(tmpOutput, str_system_error_template, errorCode, val);
      break;
  }

  strcat(output, tmpOutput);
  strcat(output, "\r\n");
}

void print_errors(char* input, char* output) {
  if (RUNSTATE == SE_TERMINAL) {
    sprintf(output, "%s%s\r\n", output, str_terminal_system_error);
  } else {
    sprintf(output, "%sYou wrote: %s\r\n", output, input);
  }

  SGLIB_LIST_MAP_ON_ELEMENTS(ErrorList, errorList, mappedError, next, {
    print_error(output, mappedError->error, mappedError->val);
  });
}

void free_errors() {
  SGLIB_LIST_MAP_ON_ELEMENTS(ErrorList, errorList, mappedError, next, {
    SGLIB_LIST_DELETE(ErrorList, errorList, mappedError, next);
    free(mappedError);
  });
  lastError = NULL;
}

void create_error(enum RunState state, enum ErrorType error, char* val) {
  ErrorList* errToken = malloc(sizeof(ErrorList));

  errToken->error = error;
  strcpy(errToken->val, val);

  SGLIB_LIST_ADD_AFTER(ErrorList, lastError, errToken, next);
  if (errorList == NULL) {
    errorList = lastError;
  }
  lastError = errToken;

  RUNSTATE = state;
}
