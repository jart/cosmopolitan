/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*
 * XXX this should somehow depend on the stuff in revision.h
 */
#pragma prog 2.3,0,0,0

/* use argument expansion */
#if !defined(UTIL) && !defined(MAINWA_BUG)
#  pragma wild argv
#endif

#include <stdio.h>
#include <malloc.h>
#include <handle.h>
#include <conio.h>>
#include <locale.h>
#include <sys/types.h>
#include "theos/stat.h" /* use JMD's extended stat function */

#define isatty() _isatty() /* THEOS isatty uses a FILE* instead of a fd */
int _isatty(int fd);

#define deletedir(d) rmdir(d)

/* will come later */
#if 0
#define USE_EF_UT_TIME          /* Enable use of "UT" extra field time info */
#endif

#if DEBUG
int _fprintf(FILE* fp, const char* fmt, ...);
#endif

/* for rename() replacement. Standard function is bugged */

int _rename(const char* old, const char* new);
#define rename(a,b) _rename(a,b)

#ifdef LOCATE_BUG
/* for fopen replacement. Standard function fails on relative path pointing
 * to root directory.
 */
#undef _fopen
#undef open
#define fopen() _fopen()
FILE* _fopen(const char*, const char*);
#define open() __open()
FILE* __open(const char*, int);
#endif

#define EXDEV   10000
