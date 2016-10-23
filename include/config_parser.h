#ifndef CONFIG_PARSER
#define CONFIG_PARSER

#define CONFIGFILEBUFFSIZE 240
#define MAXINSTRUCTIONSIZE 512

#define INT_AS_STRING_MAX_CHARS 8

#endif

void con_getVal(char* name);
void con_error(enum ErrorType error);

int con_peek(enum jsmntype_t type);
int con_peekVal(char* s);
int con_accept(enum jsmntype_t type);
int con_acceptVal(char* s);
void con_acceptUntilVal(char* val);
void con_readtok(void);
void con_object(void);
void con_objects(void);
int con_type(void);
int con_name(void);
int con_description(void);
void con_item(void);
void con_item_actions(Item* item);
void con_location_items(Location* location);
void con_location_exits(void);
void con_location(void);
void con_action(void);

void con_fill_exits(int len);
void parseConfigFile(char* filename);
