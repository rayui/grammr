#ifndef PARSER
#define PARSER

typedef struct NameList {
  char name[128];
  struct NameList* next;
} NameList;

#endif

void parser_readtok(void);

void parser_set_action_reg(void);
void parser_concat_action_reg(char* split);
void parser_set_word_reg(void);
void parser_empty_word_reg(void);

void parser_empty_name_list(void);
void parser_add_word_reg_to_name_list(void);

int parser_peek(enum TokenType type);
int parser_accept(enum TokenType type);

void parser_error(enum ErrorType error, char* val);

void parser_item(void);
void parser_action(char* output);

void parser_command(char* output);

void parser_quit(void);

void parse(Token** tokens, char* output);
