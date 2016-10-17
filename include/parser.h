#ifndef PARSER
#define PARSER

#define MAXARGSIZE 32

typedef struct NameList {
  char name[MAXARGSIZE];
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
void parser_action(InstructionList** instructions);

void parser_command(InstructionList** instructions);
void parser_commands(InstructionList** instructions);

void parser_quit(void);
void parser_eol(void);

enum RunState parse(Token** tokens, InstructionList** instructions);
