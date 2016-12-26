#ifndef CONFIG_PARSER
#define CONFIG_PARSER

#define CONFIGFILEBUFFSIZE 256
#define OBJECT_ITEM 0
#define OBJECT_ACTION 1
#define OBJECT_LOCATION 2

#endif

void parseConfigFile(char* filename);
char readOneByte(void);
long readNBytes(char* str, long n);
void readItem(void);  
void readAction(void);
void readLocation(void);
