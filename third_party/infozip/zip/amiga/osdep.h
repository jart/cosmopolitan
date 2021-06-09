/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __amiga_osdep_h
#define __amiga_osdep_h

/* default to MEDIUM_MEM, but allow makefile override */
#if ( (!defined(BIG_MEM)) && (!defined(SMALL_MEM)))
#  define MEDIUM_MEM
#endif

/* check that TZ environment variable is defined before using UTC times */
#if (!defined(NO_IZ_CHECK_TZ) && !defined(IZ_CHECK_TZ))
#  define IZ_CHECK_TZ
#endif

#ifndef IZTZ_GETLOCALETZINFO
#  define IZTZ_GETLOCALETZINFO GetPlatformLocalTimezone
#endif

/* AmigaDOS can't even support disk partitions over 4GB, let alone files */
#define NO_LARGE_FILE_SUPPORT
#ifdef LARGE_FILE_SUPPORT
#  undef LARGE_FILE_SUPPORT
#endif

#define USE_CASE_MAP
#define USE_EF_UT_TIME
#define HANDLE_AMIGA_SFX
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, 1))
#define EXIT(e) ClearIOErr_exit(e)
void ClearIOErr_exit(int e);

#include "amiga/z-stat.h"

#ifdef __SASC
#  include <sys/types.h>
#  include <exec/execbase.h>
#  if (defined(_M68020) && (!defined(__USE_SYSBASE)))
                            /* on 68020 or higher processors it is faster   */
#    define __USE_SYSBASE   /* to use the pragma libcall instead of syscall */
#  endif                    /* to access functions of the exec.library      */
#  include <proto/exec.h>   /* see SAS/C manual:part 2,chapter 2,pages 6-7  */
#  include <proto/dos.h>
#  if (defined(_M68020) && !defined(UNALIGNED_OK))
#     define UNALIGNED_OK
#  endif
#  ifndef REENTRANT
#    define REENTRANT
#  endif
#  if (defined(_NEAR_DATA) && !defined(DYN_ALLOC))
#    define DYN_ALLOC
#  endif
#  ifdef DEBUG
#    include <sprof.h>      /* profiler header file */
#  endif
#  ifndef IZTZ_SETLOCALTZINFO
     /*  XXX !!  We have really got to find a way to operate without these. */
#    define IZTZ_SETLOCALTZINFO
#  endif

/*
 A word on short-integers and SAS/C (a bug of [mc]alloc?)
 Using short integers (i.e. compiling with option SHORT-INTEGERS) is
 *not* recommended. To get maximum compression ratio the window size stored
 in WSIZE should be 32k (0x8000). However, since the size of the window[]
 array is 2*WSIZE, 65536 bytes must be allocated. The calloc function can
 only allocate UINT_MAX (defined in limits.h) bytes which is one byte short
 (65535) of the maximum window size if you are compiling with short-ints.
 You'll get an error message "Out of memory (window allocation)" whenever
 you try to deflate. Note that the compiler won't produce any warning.
 The maximum window size with short-integers is therefore 32768 bytes.
 The following is only implemented to allow the use of short-integers but
 it is once again not recommended because of a loss in compression ratio.
*/
#  if (defined(_SHORTINT) && !defined(WSIZE))
#    define WSIZE 0x4000        /* only half of maximum window size */
#  endif                        /* possible with short-integers     */
#endif /* __SASC */
/*
 With Aztec C, using short integers imposes no size limits and makes
 the program run faster, even with 32 bit CPUs, so it's recommended.
*/
#ifdef AZTEC_C
#  define NO_UNISTD_H
#  define NO_RMDIR
#  define BROKEN_FSEEK
#  ifndef IZTZ_DEFINESTDGLOBALS
#    define IZTZ_DEFINESTDGLOBALS
#  endif
#endif

extern int real_timezone_is_set;
void tzset(void);
#define VALID_TIMEZONE(tempvar) (tzset(), real_timezone_is_set)

#ifdef ZCRYPT_INTERNAL
#  ifndef CLIB_EXEC_PROTOS_H
     void *FindTask(void *);
#  endif
#  define ZCR_SEED2     (unsigned)(ulg)FindTask(NULL)
#endif

int Agetch(void);               /* getch() like function, in amiga/filedate.c */
char *GetComment(char *);

#define FOPR "rb"
#define FOPM "rb+"
#define FOPW "wb"
/* prototype for ctrl-C trap function */
void _abort(void);

#endif /* !__amiga_osdep_h */
