#define MAX_INST_ARG_SIZE 128
#define CON_SPLIT_INSTR_CHAR ";\0"
#define CON_SPLIT_ARG_CHAR ','

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

enum Instruction inst_get_instruction(char* instruction);
char parseNumArgsFromInstructions(char* instructions);
struct InstructionList* createInstructionList(enum Instruction fn, char* arg1, char* arg2);
void cc_convertSpecialVariable(char* arg, char* direct, char* indirect);
InstructionList* cc_push_instructions(char* instructions, InstructionList* last, char* direct, char* indirect);
InstructionList* cc_create_instruction(char* instructionStr, char* direct, char* indirect);