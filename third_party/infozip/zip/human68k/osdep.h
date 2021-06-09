/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include <io.h>
#include <sys/stat.h>
#include <sys/xglob.h>

#ifdef ZCRYPT_INTERNAL
#  include <process.h>          /* getpid() declaration for srand seed */
#endif

#define USE_CASE_MAP

#define ROUNDED_TIME(time)  (((time) + 1) & (~1))

#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, 1))

#ifdef HAVE_MBCTYPE_H
#  include <mbctype.h>
#else
#  define ismbblead(c) (0x80 <= (c) && ((c) < 0xa0 || 0xe0 <= (c)))
#endif
