#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sglib.h"
#include "../include/io.h"
#include "../include/jsmn.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/lexer.h"
#include "../include/items.h"
#include "../include/locations.h"
#include "../include/actions.h"
#include "../include/config_parser.h"

FILE* fp;
size_t fp_bytes;

int con_tok_num;
char buf[CONFIGFILEBUFFSIZE];
char *source;
int source_len = 0;
int con_counter = 0;
jsmntok_t* con_tok;

char con_int_reg;
char con_int_reg_b;
char con_word_reg_a[MAX_ITEM_NAME_LENGTH];
char con_word_reg_b[MAX_ITEM_NAME_LENGTH];
char con_word_reg_c[MAXINSTRUCTIONSIZE];

enum RunState CON_ERR;
extern ErrorList* errorList;
extern Item* items;
extern Location* locations;
extern Actions* actions;

void con_set_int_reg() {
  char int_reg[MAXARGSIZE];
  sprintf(int_reg, "%.*s", con_tok[con_counter].end - con_tok[con_counter].start, source + con_tok[con_counter].start);
  con_int_reg = atoi(int_reg);
}

void con_set_int_reg_b() {
  char int_reg[MAXARGSIZE];
  sprintf(int_reg, "%.*s", con_tok[con_counter].end - con_tok[con_counter].start, source + con_tok[con_counter].start);
  con_int_reg_b = atoi(int_reg);
}

void con_set_word_reg_a() {
  sprintf(con_word_reg_a, "%.*s", con_tok[con_counter].end - con_tok[con_counter].start, source + con_tok[con_counter].start);
}

void con_set_word_reg_b() {
  sprintf(con_word_reg_b, "%.*s", con_tok[con_counter].end - con_tok[con_counter].start, source + con_tok[con_counter].start);
}

void con_set_word_reg_c() {
  int size = con_tok[con_counter].end - con_tok[con_counter].start;
  if (size > MAXINSTRUCTIONSIZE) {
    con_error(CON_OVERSIZE_INSTRUCTION);
  } else {
    sprintf(con_word_reg_c, "%.*s", size, source + con_tok[con_counter].start);
  }
}

int con_set_word_reg_by_index(int reg) {
  if (reg == 0) {
    con_set_word_reg_a();
  } else if (reg == 1) {
    con_set_word_reg_b();
  } else if (reg == 2) {
    con_set_word_reg_c();
  }

  return reg <= 2;
}

void con_error(enum ErrorType error) {
  CON_ERR = SE_TERMINAL;
  con_set_word_reg_a();
  create_error(error, con_word_reg_a);
}

int con_compare_word_reg(char* s, char* reg) {
  if (strcmp(reg, s) == 0) {
    return 1;
  }
  return NULL;
}

int con_peek(enum jsmntype_t type) {
  if (con_tok[con_counter].type == type) {
    return 1;
  }
  return NULL;
}

int con_peekVal(char* s) {
  if (strncmp(source + con_tok[con_counter].start, s, strlen(s)) == 0) {
    return 1;
  }
  return NULL;
}


int con_accept(enum jsmntype_t type) {
  if (con_peek(type)) {
    con_readtok();
    return 1;
  }
  return NULL;
}

int con_acceptVal(char* s) {
  if (con_peekVal(s)) {
    con_readtok();
    return 1;
  }
  return NULL;
}

void con_acceptUntilVal(char* val) {
  while(!con_acceptVal(val)) {
    con_readtok();
  }
}

void con_readtok() {
  con_counter++;
}

int con_type() {
  if (con_acceptVal("type")) {
    if (con_peek(JSMN_STRING)) {
      con_set_word_reg_a();
      con_accept(JSMN_STRING);
      return 1;
    }
  }

  return NULL;
}

int con_name() {
  if (con_acceptVal("name")) {
    if (con_peek(JSMN_STRING)) {
      con_set_word_reg_b();
      con_accept(JSMN_STRING);
      return 1;
    }
  }

  return NULL;
}

int con_id() {
  if (con_acceptVal("id")) {
    if (con_peek(JSMN_PRIMITIVE)) {
      con_set_int_reg();
      con_accept(JSMN_PRIMITIVE);
      return 1;
    }
  }

  return NULL;
}

int con_default() {
  if (con_acceptVal("default")) {
    if (con_peek(JSMN_PRIMITIVE)) {
      con_set_int_reg();
      con_accept(JSMN_PRIMITIVE);
      return 1;
    }
  }

  return NULL;
}

int con_description() {
  if (con_acceptVal("description")) {
    if (con_peek(JSMN_STRING)) {
      con_set_word_reg_c();
      con_accept(JSMN_STRING);
      return 1;
    }
  }

  return NULL;
}

int con_instruction() {
  if (con_acceptVal("instruction")) {
    if (con_peek(JSMN_STRING)) {
      con_set_word_reg_c();
      con_accept(JSMN_STRING);
      return 1;
    }
  }

  return NULL;
}

int con_action_argument(int reg) {
  if (con_acceptVal("arg")) {
    if (con_peek(JSMN_STRING)) {
      con_set_word_reg_by_index(reg);
      con_accept(JSMN_STRING);
      return 1;
    }
  }

  return NULL;
}

void con_item_actions(char* actions) {
  int i = 0;
  int arrLen = 0;

  if (con_peek(JSMN_ARRAY)) {
    arrLen = con_tok[con_counter].size;
    if (arrLen <= MAXINSTRUCTIONS) {
      con_accept(JSMN_ARRAY);
      for(i = 0; i < arrLen; i++) {
        if (con_peek(JSMN_PRIMITIVE)) {
          con_set_int_reg();
          con_accept(JSMN_PRIMITIVE);
          actions[i] = con_int_reg;
        } else {
          con_error(CON_NUMER_EXPECTED);
        }
      }
    } else {
      con_error(CON_TOO_MANY_ACTIONS);
    }
  } else {
    con_error(CON_OBJECT_EXPECTED);
  }
}

void con_item() {
  Item* item;

  if (con_name()) {
    if (con_description()) {
      item = createItem(con_word_reg_b, con_word_reg_c);
      if (con_acceptVal("actions")) {
        con_item_actions(item->actions);
      }
    } else {
      con_error(CON_DESCRIPTION_EXPECTED);
    }
  } else {
    con_error(CON_NAME_EXPECTED);
  }
}

void con_location_items(Location* location) {
  Item* foundItem;
  int i = 0;
  int arrLen = 0;
  int len = 0;

  if (con_peek(JSMN_ARRAY)) {
    arrLen = con_tok[con_counter].size;
    con_accept(JSMN_ARRAY);
    for(i = 0; i < arrLen; i++) {
      if (con_peek(JSMN_STRING)) {
        con_set_word_reg_b();
        foundItem = findItemByName(con_word_reg_b);
        if (foundItem != NULL) {
          createItemList(&(location->items), foundItem);
        } else {
          con_error(CON_ITEM_NOT_FOUND);
        }
        con_accept(JSMN_STRING);
      } else {
        con_error(CON_NAME_EXPECTED);
        break;
      }
    }
  } else {
    con_error(CON_ARRAY_EXPECTED);
  }
}

void con_location_exits() {
  int i = 0;
  int arrLen = 0;

  if (con_peek(JSMN_ARRAY)) {
    arrLen = con_tok[con_counter].size;
    con_accept(JSMN_ARRAY);
    for(i = 0; i < arrLen; i++) {
      printOutput("%s", );
      if (con_accept(JSMN_STRING)) {
        //loop
        //we fill in the locations later
      } else {
        con_error(CON_NAME_EXPECTED);
        break;
      }
    }
  } else {
    con_error(CON_ARRAY_EXPECTED);
  }
}

void con_location() {
  Location* location;

  if (con_name()) {
    if (con_description()) {
      location = createLocation(con_word_reg_b, con_word_reg_c, NULL, NULL);

      SGLIB_LIST_ADD(Location, locations, location, next);

      if (con_acceptVal("items")) {
        con_location_items(location);
      }

      if (con_acceptVal("exits")) {
        con_location_exits();
      }

    } else {
      con_error(CON_DESCRIPTION_EXPECTED);
    }
  } else {
    con_error(CON_NAME_EXPECTED);
  }
}

void con_action() {
  Actions* action;
  char id;
  char isDefault = 0;

  if (con_id()) {
    id = con_int_reg;
    if (con_name()) {
      isDefault = con_default() ? con_int_reg : 0;
      if (con_instruction()) {
        action = createAction(id, con_word_reg_b, con_word_reg_c, isDefault);
        SGLIB_LIST_ADD(Actions, actions, action, next);
      }
    }
  }
}

void con_object() {
  if (con_type()) {
    if (con_compare_word_reg(con_word_reg_a, "item")) {
      con_item();
    } else if (con_compare_word_reg(con_word_reg_a, "location")) {
      con_location();
    } else if (con_compare_word_reg(con_word_reg_a, "action")) {
      con_action();
    } else {
      con_error(CON_OBJECT_EXPECTED);
    }
  }
}

void con_objects() {
  if (con_accept(JSMN_OBJECT)) {
    con_object();
  } else {
    con_error(CON_UNEXPECTED_TOKEN);
  }
}

void con_fill_exits(int len) {
  char arrLen = 0;
  char i = 0;
  Location* origin;
  Location* doorway;
  con_counter = 0;

  while (con_counter < len && CON_ERR == SE_OK) {
    con_acceptUntilVal("location");
    con_acceptUntilVal("name");
    con_set_word_reg_a();
    origin = findLocationByName(con_word_reg_a);
    if (origin != NULL) {
      con_accept(JSMN_STRING);
      con_acceptUntilVal("exits");
      if (con_peek(JSMN_ARRAY)) {
        arrLen = con_tok[con_counter].size;
        con_accept(JSMN_ARRAY);
        for(i = 0; i < arrLen; i++) {
          if (con_peek(JSMN_STRING)) {
            con_set_word_reg_b();
            doorway = findLocationByName(con_word_reg_b);
            createLocationList(&(origin->exits), doorway);
            con_accept(JSMN_STRING);
          } else {
            con_error(CON_NAME_EXPECTED);
            break;
          }
        }
      }
    }
  }
}

enum RunState parseConfigFile(char* filename) {
  jsmn_parser p;

  CON_ERR = SE_OK;
  con_counter = 0;

  fp = fopen(filename, "r");
  if (!fp) {
    return SE_TERMINAL;
  }

  source = malloc((CONFIGFILEBUFFSIZE) * sizeof(char));
  memset(source, 0, CONFIGFILEBUFFSIZE);

  for(;;) {
    fp_bytes = fread(buf, sizeof(char), CONFIGFILEBUFFSIZE, fp);

    if (fp_bytes < 0) {
      CON_ERR = SE_TERMINAL;
      create_error(CON_FILE_ERROR, NULL);
      break;
    } else if (fp_bytes == 0) {
      //could be an error but ignore, just break
      break;
    }

    source = realloc_it(source, source_len + fp_bytes + 1);

    if (source == NULL) {
      CON_ERR = SE_TERMINAL;
      create_error(CON_NO_MEMORY, buf);
      break;
    }

    strncpy(source + source_len, buf, fp_bytes);
    source_len += fp_bytes;

    if (fp_bytes < CONFIGFILEBUFFSIZE) {
      break;
    }
  }

  fclose(fp);

  toC64Case(source);

  if (CON_ERR != SE_TERMINAL) {
    //find out how many tokens we need to parse this file and create them
    jsmn_init(&p);
    con_tok_num = jsmn_parse(&p, source, source_len, NULL, 0);
    con_tok = malloc(con_tok_num * sizeof(jsmntok_t));

    //now parse the file into it
    jsmn_init(&p);
    jsmn_parse(&p, source, source_len, con_tok, con_tok_num);
    while (con_counter < con_tok_num && CON_ERR == SE_OK) {
      con_objects();
    }

    SGLIB_LIST_REVERSE(Actions, actions, next);

    //now fill in the exits
    con_fill_exits(con_tok_num);

    SGLIB_LIST_REVERSE(Location, locations, next);

    free(con_tok);
  }

  //clean up
  free(source);

  return CON_ERR;
}
