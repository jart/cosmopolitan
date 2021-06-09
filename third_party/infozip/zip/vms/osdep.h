/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef VMS
#  define VMS 1
#endif

#if (defined(__VMS_VER) && !defined(__CRTL_VER))
#  define __CRTL_VER __VMS_VER
#endif

#if (defined(__VMS_VERSION) && !defined(VMS_VERSION))
#  define VMS_VERSION __VMS_VERSION
#endif

#if !(defined(__DECC) || defined(__DECCXX) || defined(__GNUC__))
     /* VAX C does not properly support the void keyword. (Only functions
        are allowed to have the type "void".)  */
#  ifndef NO_TYPEDEF_VOID
#    define NO_TYPEDEF_VOID
#  endif
#  define NO_FCNTL_H        /* VAXC does not supply fcntl.h. */
#endif /* VAX C */

#define USE_CASE_MAP
#define PROCNAME(n) \
 (((action == ADD) || (action == UPDATE) || (action == FRESHEN)) ? \
 wild(n) : procname(n, filter_match_case))

/* 2004-11-09 SMS.
   Large file support.
*/
#ifdef LARGE_FILE_SUPPORT

#  define _LARGEFILE                   /* Define the pertinent macro. */

/* LARGE_FILE_SUPPORT implies ZIP64_SUPPORT,
   unless explicitly disabled by NO_ZIP64_SUPPORT.
*/
#  ifdef NO_ZIP64_SUPPORT
#    ifdef ZIP64_SUPPORT
#      undef ZIP64_SUPPORT
#    endif /* def ZIP64_SUPPORT */
#  else /* def NO_ZIP64_SUPPORT */
#    ifndef ZIP64_SUPPORT
#      define ZIP64_SUPPORT
#    endif /* ndef ZIP64_SUPPORT */
#  endif /* def NO_ZIP64_SUPPORT */

#  define ZOFF_T_FORMAT_SIZE_PREFIX "ll"

#else /* def LARGE_FILE_SUPPORT */

#  define ZOFF_T_FORMAT_SIZE_PREFIX "l"

#endif /* def LARGE_FILE_SUPPORT */

/* Need _LARGEFILE for types.h. */

#include <types.h>

#ifdef __GNUC__
#include <sys/types.h>
#endif /* def __GNUC__ */

/* Need types.h for off_t. */

#ifdef LARGE_FILE_SUPPORT
   typedef off_t zoff_t;
   typedef unsigned long long uzoff_t;
#else /* def LARGE_FILE_SUPPORT */
   typedef long zoff_t;
   typedef unsigned long uzoff_t;
#endif /* def LARGE_FILE_SUPPORT */

#include <stat.h>

typedef struct stat z_stat;

#include <unixio.h>

#if defined(__GNUC__) && !defined(ZCRYPT_INTERNAL)
#  include <unixlib.h>          /* ctermid() declaration needed in ttyio.c */
#endif
#ifdef ZCRYPT_INTERNAL
#  include <unixlib.h>          /* getpid() declaration for srand seed */
#endif

#if defined(_MBCS)
#  undef _MBCS                 /* Zip on VMS does not support MBCS */
#endif

/* VMS is run on little-endian processors with 4-byte ints:
 * enable the optimized CRC-32 code */
#ifdef IZ_CRC_BE_OPTIMIZ
#  undef IZ_CRC_BE_OPTIMIZ
#endif
#if !defined(IZ_CRC_LE_OPTIMIZ) && !defined(NO_CRC_OPTIMIZ)
#  define IZ_CRC_LE_OPTIMIZ
#endif
#if !defined(IZ_CRCOPTIM_UNFOLDTBL) && !defined(NO_CRC_OPTIMIZ)
#  define IZ_CRCOPTIM_UNFOLDTBL
#endif

#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  if (defined(__CRTL_VER) && (__CRTL_VER >= 70000000))
#    define USE_EF_UT_TIME
#  endif
#endif

#if defined(VMS_PK_EXTRA) && defined(VMS_IM_EXTRA)
#  undef VMS_IM_EXTRA                 /* PK style takes precedence */
#endif
#if !defined(VMS_PK_EXTRA) && !defined(VMS_IM_EXTRA)
#  define VMS_PK_EXTRA 1              /* PK style VMS support is default */
#endif

/* 2007-02-22 SMS.
 * <unistd.h> is needed for symbolic link functions, so use it when the
 * symbolic link criteria are met.
 */
#if defined(__VAX) || __CRTL_VER < 70301000
#  define NO_UNISTD_H
#  define NO_SYMLINKS
#endif /* defined(__VAX) || __CRTL_VER < 70301000 */

/* 2007-02-22 SMS.  Use delete() when unlink() is not available. */
#if defined(NO_UNISTD_H) || (__CRTL_VER < 70000000)
#  define unlink delete
#endif /* defined(NO_UNISTD_H) || __CRTL_VER < 70000000) */

#define SSTAT vms_stat
#define EXIT(exit_code) vms_exit(exit_code)
#define RETURN(exit_code) return (vms_exit(exit_code), 1)


#ifdef __DECC

/* File open callback ID values. */

#  define FOPM_ID 1
#  define FOPR_ID 2
#  define FOPW_ID 3

/* File open callback ID storage. */

extern int fopm_id;
extern int fopr_id;
extern int fopw_id;

/* File open callback ID function. */

extern int acc_cb();

/* Option macros for zfopen().
 * General: Stream access
 * Output: fixed-length, 512-byte records.
 *
 * Callback function (DEC C only) sets deq, mbc, mbf, rah, wbh, ...
 */

#  define FOPM "r+b", "ctx=stm", "rfm=fix", "mrs=512", "acc", acc_cb, &fopm_id
#  define FOPR "rb",  "ctx=stm", "acc", acc_cb, &fopr_id
#  define FOPW "wb",  "ctx=stm", "rfm=fix", "mrs=512", "acc", acc_cb, &fopw_id

#else /* def __DECC */ /* (So, GNU C, VAX C, ...)*/

#  define FOPM "r+b", "ctx=stm", "rfm=fix", "mrs=512"
#  define FOPR "rb",  "ctx=stm"
#  define FOPW "wb",  "ctx=stm", "rfm=fix", "mrs=512"

#endif /* def __DECC */

