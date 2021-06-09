/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "wb"

#define DIRENT
#define NO_TERMIO
#define USE_CASE_MAP
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, 1))

#include <sys/types.h>
#include <sys/stat.h>
