/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/* Include file for VM/CMS and MVS */

/* This is normally named osdep.h on most systems.  Since CMS       */
/* generally doesn't support directories, it's been given a unique  */
/* name to avoid confusion.                                         */


#ifndef __cmsmvs_h   /* prevent multiple inclusions */
#define __cmsmvs_h

#ifdef MVS
#  define _POSIX_SOURCE    /* tell MVS we want full definitions */
#  include <features.h>
#endif /* MVS */

#include <time.h>               /* the usual non-BSD time functions */
/* cstat.h is not required for MVS and actually gets in the way.  Is it
 * needed for CMS?
 */
#ifdef MVS
#  include <sys/stat.h>
#  include <sys/modes.h>
#else /* !MVS */
#  include "cstat.h"
#endif


/* Newer compiler version defines something for us */
#if defined(__VM__) && !defined(VM_CMS)
#  define VM_CMS
#endif

#define CMS_MVS
#define EBCDIC

#ifndef MVS  /* MVS has perfectly good definitions for the following */
#  define NO_UNISTD_H
#  define NO_FCNTL_H
#endif /*MVS */

/* If we're generating a stand-alone CMS module, patch in        */
/* a new main() function before the real main() for arg parsing. */
#ifdef CMS_STAND_ALONE
#  define USE_ZIPMAIN
#endif

#ifndef NULL
#  define NULL 0
#endif

#define PASSWD_FROM_STDIN
                  /* Kludge until we know how to open a non-echo tty channel */

/* definition for ZIP */
#define getch() getc(stdin)
#define MAXPATHLEN 128
#define NO_RMDIR
#define NO_MKTEMP
#define USE_CASE_MAP
#define isatty(t) 1

#ifndef MVS  /* MVS has perfectly good definitions for the following */
#  define fileno(x) (char *)(x)
#  define fdopen fopen
#  define unlink remove
#  define link rename
#  define utime(f,t)
#endif /*MVS */
#ifdef ZCRYPT_INTERNAL
#  define ZCR_SEED2     (unsigned)3141592654L   /* use PI as seed pattern */
#endif

#ifdef MVS
#  if defined(__CRC32_C)
#    pragma csect(STATIC,"crc32_s")
#  elif defined(__DEFLATE_C)
#    pragma csect(STATIC,"deflat_s")
#  elif defined(__ZIPFILE_C)
#    pragma csect(STATIC,"zipfil_s")
#  elif defined(__ZIPUP_C)
#    pragma csect(STATIC,"zipup_s")
#  endif
#endif /* MVS */

/* end defines for ZIP */



#if 0  /*$RGH$*/
/* RECFM=F, LRECL=1 works for sure */
#define FOPR "rb,recfm=fb"
#define FOPM "r+"
#define FOPW "wb,recfm=fb,lrecl=1"
#define FOPWT "w"
#endif

/* Try allowing ZIP files to be RECFM=V with "byteseek" for CMS, recfm=U for MVS */
#define FOPR "rb,byteseek"
#define FOPM "r+,byteseek"
#ifdef MVS
  #define FOPW "wb,recfm=u,byteseek"
#else /* !MVS */
  #define FOPW "wb,recfm=v,lrecl=32760,byteseek"
#endif /* MVS */

#if 0
#define FOPW_TMP "w,byteseek"
#else
#define FOPW_TMP "w,type=memory(hiperspace)"
#endif

#define CBSZ 0x40000
#define ZBSZ 0x40000

#endif /* !__cmsmvs_h */
