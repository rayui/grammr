#ifndef INTERPRETER
#define INTERPRETER

#define SKIP_NONE 0
#define SKIP_ONE 1
#define SKIP_GOTO 2
#define SKIP_END 3

#define MAXEXITSTEXTLENGTH 128
#define MAXITEMSTEXTLENGTH 128
#define MAXOUTPUTSIZE 1024

enum RunState interpret(InstructionList** instructions, char* output);
