/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#ifndef _QDOS_OPDEP
#define _QDOS_OPDEP

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

char * ql2Unix(char *);
char * Unix2ql(char *, char **);
int wild (char *);
char *LastDir(char *);
void QDOSexit(void);
short devlen(char *);

/*
 * XXX NO_RENAME instead of the following define ?
 */
#define link rename
#define USE_CASE_MAP
#define USE_EF_UT_TIME
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, 1))
#endif
