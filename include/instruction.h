#define MAX_INSTRUCTION_LENGTH 512
#define CON_SPLIT_INSTR_CHAR ";\0"
#define CON_SPLIT_ARG_CHAR ','

enum Instruction {
  INST_INVALID,
  INST_SET_PARAMS,
  INST_EQ,
  INST_ITEMINLOCATION,
  INST_ITEMININVENTORY,
  INST_ITEMINCONTEXT,
  INST_HASEXIT,
  INST_NOT,
  INST_IF,
  INST_SETLOC,
  INST_ADDITEM,
  INST_DELITEM,
  INST_RETURN,
  INST_GOTO,
  INST_GOTO_IF,
  INST_LABEL,
  INST_PRINT,
  INST_PRINTDESC,
  INST_PRINTEXITS,
  INST_PRINTACTIONS,
  INST_PRINTITEMS,
  INST_NEWLINE,
  INST_ACTION,
  INST_DEBUG
};

typedef struct InstructionList {
  enum Instruction fn;
  char *arg1;
  char *arg2;
  struct InstructionList* next;
} InstructionList;

enum Instruction inst_get_instruction_code(char* instruction);
void free_instructions(InstructionList* instructions);
InstructionList* inst_set_params(InstructionList* last, char* direct, char* indirect);
InstructionList* inst_create(char* instructionStr);
InstructionList* inst_insert(char* newInstructions, InstructionList* last);