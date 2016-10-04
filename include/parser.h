#ifndef PARSER
#define PARSER

#define MAXARGSIZE 32

typedef struct NameList {
  char name[MAXARGSIZE];
  struct NameList* next;
} NameList;

#endif

void cc_empty_name_list(void);
void cc_set_word_reg(void);

void cc_push_to_cc_word_reg(void);

void cc_commands(void);
void cc_command(void);
void cc_eol(void);
void cc_quit(void);

void cc_item(void);
void cc_items(void);
void cc_words(void);
void cc_concat_words(void);

void cc_go(void);

void cc_at(void);
void cc_in(void);
void cc_with(void);
void cc_on(void);

void cc_error(enum ErrorType error, char* val);

int cc_peek(enum TokenType type);
int cc_peakVal(char *s);
int cc_accept(enum TokenType type);
int cc_acceptVal(char *s);
void cc_expect(enum TokenType type);
void cc_readtok(void);

void free_parser(void);
enum RunState parse(void);
