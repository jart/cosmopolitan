/*
  unix/osdep.h - Zip 3

  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

#ifdef NO_LARGE_FILE_SUPPORT
# ifdef LARGE_FILE_SUPPORT
#  undef LARGE_FILE_SUPPORT
# endif
#endif

#ifdef LARGE_FILE_SUPPORT
  /* 64-bit Large File Support */

  /* The following Large File Summit (LFS) defines turn on large file support on
     Linux (probably 2.4 or later kernel) and many other unixen */

# define _LARGEFILE_SOURCE      /* some OSes need this for fseeko */
# define _LARGEFILE64_SOURCE
# define _FILE_OFFSET_BITS 64   /* select default interface as 64 bit */
# define _LARGE_FILES           /* some OSes need this for 64-bit off_t */
#endif

#include <sys/types.h>
#include <sys/stat.h>

/* printf format size prefix for zoff_t values */
#ifdef LARGE_FILE_SUPPORT
# define ZOFF_T_FORMAT_SIZE_PREFIX "ll"
#else
# define ZOFF_T_FORMAT_SIZE_PREFIX "l"
#endif

#ifdef NO_OFF_T
  typedef long zoff_t;
  typedef unsigned long uzoff_t;
#else
  typedef off_t zoff_t;
# if defined(LARGE_FILE_SUPPORT) && !(defined(__alpha) && defined(__osf__))
  typedef unsigned long long uzoff_t;
# else
  typedef unsigned long uzoff_t;
# endif
#endif
  typedef struct stat z_stat;


/* Automatically set ZIP64_SUPPORT if LFS */

#ifdef LARGE_FILE_SUPPORT
# ifndef NO_ZIP64_SUPPORT
#   ifndef ZIP64_SUPPORT
#     define ZIP64_SUPPORT
#   endif
# else
#   ifdef ZIP64_SUPPORT
#     undef ZIP64_SUPPORT
#   endif
# endif
#endif


/* Process files in binary mode */
#if defined(__DJGPP__) || defined(__CYGWIN__)
#  define FOPR "rb"
#  define FOPM "r+b"
#  define FOPW "wb"
#endif


/* Enable the "UT" extra field (time info) */
#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  define USE_EF_UT_TIME
#endif
