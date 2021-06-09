/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#ifndef __amiga_amiga_h
#define __amiga_amiga_h

/* amiga.h
 *
 * Globular definitions that affect all of AmigaDom.
 *
 * Originally included in unzip.h, extracted for simplicity and eeze of
 * maintenance by John Bush.
 *
 * This version is for use with Zip.  It is not globally included, but used
 * only by functions in amiga/amigazip.c.  Much material that was needed for
 * UnZip is absent here.
 *
 */

#include <fcntl.h>          /* O_BINARY for open() w/o CR/LF translation */
#include "amiga/z-stat.h"  /* substitute for <stat.h> and <dire(c|n)t.h> */
#define direct dirent

#ifndef MODERN
#  define MODERN
#endif

#ifdef AZTEC_C                          /* Manx Aztec C, 5.0 or newer only */
#  include <clib/dos_protos.h>
#  include <pragmas/dos_lib.h>          /* do inline dos.library calls */
#  define O_BINARY 0
#endif /* AZTEC_C */


#ifdef __SASC
#  include <dirent.h>
#  include <dos.h>
#  define disk_not_mounted 0
#  if ( (!defined(O_BINARY)) && defined(O_RAW))
#    define O_BINARY O_RAW
#  endif
#endif /* SASC */


/* Funkshine Prough Toe Taipes */

LONG FileDate (char *, time_t[]);

#endif /* __amiga_amiga_h */
