/*
  ttyio.h - Zip 3

  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
   ttyio.h
 */

#ifndef __ttyio_h   /* don't include more than once */
#define __ttyio_h

#ifndef __crypt_h
#include "third_party/zip/crypt.h"  /* ensure that encryption header file has been seen */
#endif

#if (CRYPT || (defined(UNZIP) && !defined(FUNZIP)))
/*
 * Non-echo keyboard/console input support is needed and enabled.
 */

#ifndef __G         /* UnZip only, for now (DLL stuff) */
#  define __G
#  define __G__
#  define __GDEF
#  define __GPRO    void
#  define __GPRO__
#endif

#ifndef ZCONST      /* UnZip only (until have configure script like Zip) */
#  define ZCONST const
#endif

#if (defined(MSDOS) || defined(OS2) || defined(WIN32))
#  ifndef DOS_OS2_W32
#    define DOS_OS2_W32
#  endif
#endif

#if (defined(DOS_OS2_W32) || defined(__human68k__))
#  ifndef DOS_H68_OS2_W32
#    define DOS_H68_OS2_W32
#  endif
#endif

#if (defined(DOS_OS2_W32) || defined(FLEXOS))
#  ifndef DOS_FLX_OS2_W32
#    define DOS_FLX_OS2_W32
#  endif
#endif

#if (defined(DOS_H68_OS2_W32) || defined(FLEXOS))
#  ifndef DOS_FLX_H68_OS2_W32
#    define DOS_FLX_H68_OS2_W32
#  endif
#endif

#if (defined(__ATHEOS__) || defined(__BEOS__) || defined(UNIX))
#  ifndef ATH_BEO_UNX
#    define ATH_BEO_UNX
#  endif
#endif

#if (defined(VM_CMS) || defined(MVS))
#  ifndef CMS_MVS
#    define CMS_MVS
#  endif
#endif


/* Function prototypes */

/* For all other systems, ttyio.c supplies the two functions Echoff() and
 * Echon() for suppressing and (re)enabling console input echo.
 */
#ifndef echoff
#  define echoff(f)  Echoff(__G__ f)
#  define echon()    Echon(__G)
   void Echoff OF((__GPRO__ int f));
   void Echon OF((__GPRO));
#endif

/* this stuff is used by MORE and also now by the ctrl-S code; fileio.c only */
#if (defined(UNZIP) && !defined(FUNZIP))
#  ifdef HAVE_WORKING_GETCH
#    define FGETCH(f)  getch()
#  endif
#  ifndef FGETCH
     /* default for all systems where no getch()-like function is available */
     int zgetch OF((__GPRO__ int f));
#    define FGETCH(f)  zgetch(__G__ f)
#  endif
#endif /* UNZIP && !FUNZIP */

#if (CRYPT && !defined(WINDLL))
   char *getp OF((__GPRO__ ZCONST char *m, char *p, int n));
#endif

#else /* !(CRYPT || (UNZIP && !FUNZIP)) */

/*
 * No need for non-echo keyboard/console input; provide dummy definitions.
 */
#define echoff(f)
#define echon()

#endif /* ?(CRYPT || (UNZIP && !FUNZIP)) */

#endif /* !__ttyio_h */
