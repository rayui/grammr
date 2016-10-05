#ifndef PARSER
#define PARSER

#define MAXARGSIZE 32

typedef struct NameList {
  char name[MAXARGSIZE];
  struct NameList* next;
} NameList;

#endif

void cc_readtok(void);

void cc_set_action_reg(void);
void cc_concat_action_reg(char* split);
void cc_set_word_reg(void);
void cc_empty_word_reg(void);

void cc_empty_name_list(void);
void cc_push_to_cc_word_reg(void);

int cc_peek(enum TokenType type);
int cc_peakVal(char *s);
int cc_accept(enum TokenType type);

void cc_error(enum ErrorType error, char* val);

void cc_item(void);
void cc_action(InstructionList** instructions);

void cc_command(InstructionList** instructions);
void cc_commands(InstructionList** instructions);

void cc_quit(void);
void cc_eol(void);

void free_parser(InstructionList** instructions);
enum RunState parse(Token** tokens, InstructionList** instructions);
