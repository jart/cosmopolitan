/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#include <stdio.h>
#include <stdarg.h>

/* for Info and similar macroes. fprintf is already a macro and fprintf x
 * fools the preprocessor
 */

int _fprintf(FILE* fp, const char* fmt, ...)
{
    va_list ap;
    long n;

    va_start(ap, fmt);
    n = vfprintf(fp, fmt, (long*) ap);
    va_end(ap);
    return n;
}

