#define MAX_INST_ARG_SIZE 256
#define MAX_INSTRUCTION_LENGTH 512
#define CON_SPLIT_INSTR_CHAR ";\0"
#define CON_SPLIT_ARG_CHAR ','

enum Instruction {
  INST_INVALID,
  INST_EQ,
  INST_ITEMINLOCATION,
  INST_ITEMININVENTORY,
  INST_ITEMININVENTORYORLOCATION,
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
  INST_PRINTITEMS,
  INST_NEWLINE,
  INST_ACTION
};

typedef struct InstructionList {
  enum Instruction fn;
  char *arg1;
  char *arg2;
  struct InstructionList* next;
} InstructionList;

enum Instruction inst_get_instruction_code(char* instruction);
void inst_convert_special_variable(char* arg, char* direct, char* indirect);
void free_instructions(InstructionList* instructions);
InstructionList* inst_create(char* instructionStr, char* direct, char* indirect);
InstructionList* inst_insert(InstructionList** instructions, char* newInstructions, InstructionList* last, char* direct, char* indirect);