/*

debugmalloc (C) Per Olofsson 2004

Redistribution and use in source and binary forms, with or without
modification, are permitted as specified in the standard BSD license:

  http://www.opensource.org/licenses/bsd-license.php

*/


#define malloc(SIZE) debugmalloc(SIZE)
#define free(BLOCK) debugfree(BLOCK)


void* __fastcall__ debugmalloc(size_t size);
void __fastcall__ debugfree(void* block);
