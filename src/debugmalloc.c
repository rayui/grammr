/*

debugmalloc (C) Per Olofsson 2004

Redistribution and use in source and binary forms, with or without
modification, are permitted as specified in the standard BSD license:

  http://www.opensource.org/licenses/bsd-license.php

*/


#include <stdlib.h>
#include <string.h>
#include <conio.h>


#define CANARY1 0x55
#define CANARY2 0xaa

#define MAXCHUNKS 16

static void *chunk[MAXCHUNKS];
static unsigned char debuginit = 0;

static signed char chunks = 0;
static unsigned char x, y, c;

#define SAVEXYC(C)	do { x = wherex(); y = wherey(); c = textcolor(C); } while (0)
#define RESTOREXYC()	do { gotoxy(x, y); textcolor(c); } while (0)


void* __fastcall__ debugmalloc(size_t size) {
  unsigned char *ptr;
  unsigned char i;

  if (!debuginit) {
    ++debuginit;
    memset(chunk, 0, sizeof(chunk));
  }
  SAVEXYC(14);
  if (size == 0) {
    textcolor(10);
    cputsxy(0, 24, "malloc(0)!  ");
    cgetc();
  }
  gotoxy(28, 24);
  ptr = malloc(size + 6);
  textcolor(14);
  cprintf("f%6d c%3d", _heapmemavail(), ++chunks);
  if (ptr) {
    ptr[0] = size & 0xff;
    ptr[1] = size>>8;
    ptr[2] = CANARY1;
    ptr[3] = CANARY2;
    ptr[size + 4] = CANARY1;
    ptr[size + 5] = CANARY2;
    for (i = 0; i < MAXCHUNKS; ++i) {
      if (chunk[i] == 0) {
	chunk[i] = ptr + 4;
	RESTOREXYC();
	return(ptr + 4);
      }
    }
    textcolor(10);
    cputsxy(0, 24, "Chunk table full!  ");
    cgetc();
    RESTOREXYC();
    return(ptr + 4);
  } else {
    textcolor(10);
    cputsxy(0, 24, "Out of memory!  ");
    cgetc();
    RESTOREXYC();
    return(NULL);
  }
}


void __fastcall__ debugfree(void* block) {
  unsigned char *ptr = block;
  unsigned int len;
  unsigned char i;

  SAVEXYC(14);
  if (!debuginit) {
    textcolor(10);
    cputsxy(0, 24, "debugfree before debugmalloc!");
    cgetc();
    free(ptr - 4);
    RESTOREXYC();
    return;
  }
  for (i = 0; i < MAXCHUNKS; ++i) {
    if (chunk[i] == block) {
      chunk[i] = 0;
      break;
    }
  }
  if (i == MAXCHUNKS) {
    textcolor(10);
    cputsxy(0, 24, "free of unmalloced chunk!  ");
    cgetc();
  }
  len = ptr[-4] | (ptr[-3]<<8);
  if (block == NULL) {
    textcolor(10);
    cputsxy(0, 24, "free(NULL)!  ");
    cgetc();
  } else {
    if (ptr[-2] != CANARY1 || ptr[-1] != CANARY2) {
      textcolor(10);
      gotoxy(0, 24);
      cprintf("Mem before %04x overwritten!", (unsigned int) block);
      cgetc();
    }
    if (ptr[len] != CANARY1 || ptr[len + 1] != CANARY2) {
      textcolor(10);
      gotoxy(0, 24);
      cprintf("Mem after %04x overwritten! ", (unsigned int) block + len - 1);
      cgetc();
    }
    free((unsigned char *) block - 4);
    textcolor(14);
    gotoxy(28, 24);
    if (--chunks < 0) {
      textcolor(10);
      cputsxy(0, 24, "Free'd more than malloc'd!  ");
      cgetc();
    }
    cprintf("f%6d c%3d", _heapmemavail(), chunks);
  }
  RESTOREXYC();
}
