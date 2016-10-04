#define MAX_INST_ARG_SIZE 128

enum Instruction {
  INST_INVALID,
  INST_EQ,
  INST_ITEMINLOCATION,
  INST_ITEMININVENTORY,
  INST_HASEXIT,
  INST_NOT,
  INST_IF,
  INST_SETLOC,
  INST_ADDITEM,
  INST_DELITEM,
  INST_RETURN,
  INST_GOTO,
  INST_LABEL,
  INST_PRINT,
  INST_PRINTDESC,
  INST_PRINTEXITS,
  INST_PRINTITEMS,
  INST_NEWLINE,
  INST_ACTION
};

typedef struct InstructionList {
  enum Instruction fn;
  char arg1[MAX_INST_ARG_SIZE];
  char arg2[MAX_INST_ARG_SIZE];
  struct InstructionList* next;
  struct InstructionList* prev;
} InstructionList;

typedef struct Actions {
  char id;
  char name[MAX_INST_ARG_SIZE];
  char args;
  char isDefault;
  char* instructions;
  struct Actions* next;
} Actions;

enum Instruction inst_get_instruction(char* instruction);
char parseNumArgsFromInstructions(char* instructions);
struct InstructionList* createInstructionList(enum Instruction fn, char* arg1, char* arg2);
struct Actions* createAction(char id, char* name, char* instructions, char isDefault);
Actions* findActionById(char id);
Actions* findActionByName(char* name);
Actions* findActionByNameAndItem(struct Item* item, char* name);
Actions* findDefaultActionByName(char* name);
