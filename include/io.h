#include <conio.h>
#include <stdlib.h>

#ifndef YammadoreIO_h
#define YammadoreIO_h

# define PRINT  cprintf
# define PUT(c) cputc((char)(c))

#define COMMAND_SIZE 256

void acceptInput(char** input);
/* get a command */

#endif
