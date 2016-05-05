#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/error.h"

extern const char str_syntax_error_end_of_command_expected[];
extern const char str_syntax_error_command_expected[];
extern const char str_syntax_error_word_expected[];
extern const char str_syntax_error_direction_expected[];
extern const char str_syntax_error_item_expected[];
extern const char str_syntax_error_preposition_expected[];
extern const char str_syntax_error_unrecognised_token[];
extern const char str_syntax_error_item_unavailable[];
extern const char str_syntax_error_no_such_action[];
extern const char str_terminal_system_error[];
extern const char str_system_error_template[];

extern ErrorList* errorList;

void print_error(char* output, enum ErrorType errorCode, char* val) {
  char tmpOutput[MAXERRSIZE];

  switch(errorCode) {
    case CC_ERR_END_OF_COMMAND_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_end_of_command_expected);
      break;
    case CC_ERR_COMMAND_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_command_expected);
      break;
    case CC_WORD_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_word_expected);
      break;
    case CC_DIRECTION_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_direction_expected, val);
      break;
    case CC_ITEM_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_item_expected, val);
      break;
    case CC_PREPOSITION_EXPECTED:
      sprintf(tmpOutput, str_syntax_error_preposition_expected);
      break;
    case LEX_UNRECOGNISED_TOKEN:
      sprintf(tmpOutput, str_syntax_error_unrecognised_token);
      break;
    case CC_ITEM_UNAVAILABLE:
      sprintf(tmpOutput, str_syntax_error_item_unavailable, val);
      break;
    case CC_NO_SUCH_ACTION:
      sprintf(tmpOutput, str_syntax_error_no_such_action, val);
      break;
    default:
      sprintf(tmpOutput, str_system_error_template, errorCode, val);
      break;
  }

  strcat(tmpOutput, "\r\n");
  strcat(output, tmpOutput);
}

void print_errors(enum RunState err, char* output) {
  if (err == SE_TERMINAL) {
    strcpy(output, str_terminal_system_error);
  }

  SGLIB_LIST_REVERSE(ErrorList, errorList, next);
  SGLIB_LIST_MAP_ON_ELEMENTS(ErrorList, errorList, mappedError, next, {
    print_error(output, mappedError->error, mappedError->val);
  });
}

void free_errors() {
  SGLIB_LIST_MAP_ON_ELEMENTS(ErrorList, errorList, mappedError, next, {
    SGLIB_LIST_DELETE(ErrorList, errorList, mappedError, next);
    free(mappedError);
  });
}

void create_error(enum ErrorType error, char* val) {
  ErrorList* errToken = malloc(sizeof(ErrorList));

  errToken->error = error;
  strcpy(errToken->val, val);

  SGLIB_LIST_ADD(ErrorList, errorList, errToken, next);
}
