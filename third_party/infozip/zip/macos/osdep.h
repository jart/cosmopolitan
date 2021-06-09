/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __MACOS_OSDEP_H
#define __MACOS_OSDEP_H 1

#ifndef MACOS
#  define MACOS
#endif


#include <setjmp.h>
#include <types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unix.h>
#include <unistd.h>
#include <console.h>

#include <Errors.h>
#include <Files.h>

#include "unixlike.h"
#include "macglob.h"

#define NO_MKTEMP           1
#define PASSWD_FROM_STDIN   1
#define NO_SYMLINKS         1

#define USE_ZIPMAIN         1

#define USE_CASE_MAP        1  /* case_map is used to ignore case in comparisons */




/*
#define DEBUG_TIME
 */

#if (!defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME))
#  define USE_EF_UT_TIME
#endif

#undef  IZ_CHECK_TZ

#ifndef ZP_NEED_MEMCOMPR
#  define ZP_NEED_MEMCOMPR
#endif


#define EXDEV  18

#define PATHCUT ':'


/* file operations use "b" for binary */
#define FOPR  "rb"
#define FOPM  "r+b"
#define FOPW  "wb"

/*
#define DEBUG
*/

/* These functions are defined as a macro instead of a function.
so we have to undefine them for replacing (see printf.c)  */
#undef getc
#undef getchar
#undef putchar



void setfiletype(char *file, unsigned long Creator, unsigned long Type);

char *GetZipVersionsInfo(void);
char *GetZipVersionLocal(void);
char *GetZipCopyright(void);

void InitAllVars(void);

void PrintFileInfo(void);



int fprintf(FILE *file, const char *format, ...);
int printf(const char *format, ...);
void perror(const char *parm1);
int macgetch(void);


int MacOpen(const char *path,int oflag,...);
FILE *MacFopen(const char *path,const char *mode);
#define fopen(path, mode)   MacFopen(path, mode)
#define open(path, oflag)   MacOpen(path, oflag)


char *GetComment(char *filename);
int readlink(char *path, char *buf, int size);

void PrintStatProgress(char *msg);
void InformProgress(const long progressMax, const long progressSoFar );
void ShowCounter(Boolean reset);
void leftStatusString(char *status);




#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, 1))

#endif   /* __MACOS_OSDEP_H */
