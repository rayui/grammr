#ifndef INTERPRETER
#define INTERPRETER

#define SKIP_NONE 0
#define SKIP_ONE 1
#define SKIP_GOTO 2
#define SKIP_END 3

#define MAXEXITSTEXTLENGTH 128
#define MAXITEMSTEXTLENGTH 128
#define MAXOUTPUTSIZE 1024

char* intrpt_convert_special_variable(char* arg);
void intrpt_action(char* output, InstructionList* instructions, char* arg1, char* arg2);
void intrpt_invalid(char* output, enum Instruction fn, char* arg1, char* arg2);
void intrpt_eq(char* arg1, char* arg2);
void intrpt_locationhasitem(char* arg1, char* arg2);
void intrpt_inventoryhasitem(char* arg1);
void intrprt_inventoryorcurrentlocationhasitem(char* arg1, char* arg2);
void intrpt_hasexit(char* arg1, char* arg2);
void intrpt_not(void);
void intrpt_if(void);
void intrpt_setloc(char* arg1);
void intrpt_additem(char* arg1, char* arg2);
void intrpt_delitem(char* arg1, char* arg2);
void intrpt_return(void);
void intrpt_goto(char* arg1);
void intrpt_label(void);
void intrpt_print(char* output, char* arg1);
void intrpt_printdesc(char* output, char* arg1);
void intrpt_printexits(char* output, char* arg1);
void intrpt_printitems(char* output, char* arg1);
void intrpt_newline(char* output);
void intrpt_instruction(char* output, InstructionList* instructions, InstructionList* instruction);
void interpret(InstructionList** instructions, char* output);
