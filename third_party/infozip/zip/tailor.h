/*
  tailor.h - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

/* Some compiler distributions for Win32/i386 systems try to emulate
 * a Unix (POSIX-compatible) environment.
 */
#if (defined(WIN32) && defined(UNIX))
   /* Zip does not support merging both ports in a single executable. */
#  if (defined(FORCE_WIN32_OVER_UNIX) && defined(FORCE_UNIX_OVER_WIN32))
     /* conflicting choice requests -> we prefer the Win32 environment */
#    undef FORCE_UNIX_OVER_WIN32
#  endif
#  ifdef FORCE_WIN32_OVER_UNIX
     /* native Win32 support was explicitely requested... */
#    undef UNIX
#  else
     /* use the POSIX (Unix) emulation features by default... */
#    undef WIN32
#  endif
#endif


/* UNICODE */
#ifdef NO_UNICODE_SUPPORT
# ifdef UNICODE_SUPPORT
#   undef UNICODE_SUPPORT
# endif
#endif


#ifdef AMIGA
#include "amiga/osdep.h"
#endif

#ifdef AOSVS
#include "aosvs/osdep.h"
#endif

#ifdef ATARI
#include "atari/osdep.h"
#endif

#ifdef __ATHEOS__
#include "atheos/osdep.h"
#endif

#ifdef __BEOS__
#include "beos/osdep.h"
#endif

#ifdef DOS
#include "msdos/osdep.h"
#endif

#ifdef __human68k__
#include "human68k/osdep.h"
#endif

#if ((defined(__MWERKS__) && defined(macintosh)) || defined(MACOS))
#include "macos/osdep.h"
#endif

#ifdef NLM
#include "novell/osdep.h"
#endif

#ifdef OS2
#include "os2/osdep.h"
#endif

#ifdef __riscos
#include "acorn/osdep.h"
#endif

#ifdef QDOS
#include "qdos/osdep.h"
#endif

#ifdef __TANDEM
#include "tandem.h"
#include "tanzip.h"
#endif

#ifdef UNIX
#include "unix/osdep.h"
#endif

#if defined(__COMPILER_KCC__) || defined(TOPS20)
#include "tops20/osdep.h"
#endif

#if defined(VMS) || defined(__VMS)
#include "vms/osdep.h"
#endif

#if defined(__VM__) || defined(VM_CMS) || defined(MVS)
#include "cmsmvs.h"
#endif

#ifdef WIN32
#include "win32/osdep.h"
#endif

#ifdef THEOS
#include "theos/osdep.h"
#endif


/* generic LARGE_FILE_SUPPORT defines
   These get used if not defined above.
   7/21/2004 EG
*/
/* If a port hasn't defined ZOFF_T_FORMAT_SIZE_PREFIX
   then probably need to define all of these. */
#ifndef ZOFF_T_FORMAT_SIZE_PREFIX

# ifdef LARGE_FILE_SUPPORT
    /* Probably passed in from command line instead of in above
       includes if get here.  Assume large file support and hope. 8/14/04 EG */

    /* Set the Large File Summit (LFS) defines to turn on large file support
       in case it helps. */

#   define _LARGEFILE_SOURCE    /* some OSes need this for fseeko */
#   define _LARGEFILE64_SOURCE
#   define _FILE_OFFSET_BITS 64 /* select default interface as 64 bit */
#   define _LARGE_FILES         /* some OSes need this for 64-bit off_t */

    typedef off_t zoff_t;
    typedef unsigned long long uzoff_t;  /* unsigned zoff_t (12/29/04 EG) */

    /* go with common prefix */
#   define ZOFF_T_FORMAT_SIZE_PREFIX "ll"

# else
    /* Default type for offsets and file sizes was ulg but reports
       of using ulg to create files from 2 GB to 4 GB suggest
       it doesn't work well.  Now just switch to Zip64 or not
       support over 2 GB.  7/24/04 EG */
    /* Now use uzoff_t for unsigned things.  12/29/04 EG */
    typedef long zoff_t;
    typedef unsigned long uzoff_t;

#   define ZOFF_T_FORMAT_SIZE_PREFIX "l"

# endif

  typedef struct stat z_stat;

  /* flag that we are defaulting */
# define USING_DEFAULT_LARGE_FILE_SUPPORT
#endif


#if (defined(USE_ZLIB) && defined(ASM_CRC))
#  undef ASM_CRC
#endif

#if (defined(USE_ZLIB) && defined(ASMV))
#  undef ASMV
#endif

/* When "void" is an alias for "int", prototypes cannot be used. */
#if (defined(NO_VOID) && !defined(NO_PROTO))
#  define NO_PROTO
#endif

/* Used to remove arguments in function prototypes for non-ANSI C */
#ifndef NO_PROTO
#  define OF(a) a
#  define OFT(a) a
#else /* NO_PROTO */
#  define OF(a) ()
#  define OFT(a)
#endif /* ?NO_PROTO */

/* If the compiler can't handle const define ZCONST in osdep.h */
/* Define const itself in case the system include files are bonkers */
#ifndef ZCONST
#  ifdef NO_CONST
#    define ZCONST
#    define const
#  else
#    define ZCONST const
#  endif
#endif

/*
 * Some compiler environments may require additional attributes attached
 * to declarations of runtime libary functions (e.g. to prepare for
 * linking against a "shared dll" version of the RTL).  Here, we provide
 * the "empty" default for these attributes.
 */
#ifndef IZ_IMP
#  define IZ_IMP
#endif

/*
 * case mapping functions. case_map is used to ignore case in comparisons,
 * to_up is used to force upper case even on Unix (for dosify option).
 */
#ifdef USE_CASE_MAP
#  define case_map(c) upper[(c) & 0xff]
#  define to_up(c)    upper[(c) & 0xff]
#else
#  define case_map(c) (c)
#  define to_up(c)    ((c) >= 'a' && (c) <= 'z' ? (c)-'a'+'A' : (c))
#endif /* USE_CASE_MAP */

/* Define void, zvoid, and extent (size_t) */
#include <stdio.h>

#ifndef NO_STDDEF_H
#  include <stddef.h>
#endif /* !NO_STDDEF_H */

#ifndef NO_STDLIB_H
#  include <stdlib.h>
#endif /* !NO_STDLIB_H */

#ifndef NO_UNISTD_H
#  include <unistd.h> /* usually defines _POSIX_VERSION */
#endif /* !NO_UNISTD_H */

#ifndef NO_FCNTL_H
#  include <fcntl.h>
#endif /* !NO_FNCTL_H */

#ifndef NO_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#endif /* NO_STRING_H */

#ifdef NO_VOID
#  define void int
   typedef char zvoid;
#else /* !NO_VOID */
# ifdef NO_TYPEDEF_VOID
#  define zvoid void
# else
   typedef void zvoid;
# endif
#endif /* ?NO_VOID */

#ifdef NO_STRRCHR
#  define strrchr rindex
#endif

#ifdef NO_STRCHR
#  define strchr index
#endif

/*
 * A couple of forward declarations that are needed on systems that do
 * not supply C runtime library prototypes.
 */
#ifdef NO_PROTO
IZ_IMP char *strcpy();
IZ_IMP char *strcat();
IZ_IMP char *strrchr();
/* XXX use !defined(ZMEM) && !defined(__hpux__) ? */
#if !defined(ZMEM) && defined(NO_STRING_H)
IZ_IMP char *memset();
IZ_IMP char *memcpy();
#endif /* !ZMEM && NO_STRING_H */

/* XXX use !defined(__hpux__) ? */
#ifdef NO_STDLIB_H
IZ_IMP char *calloc();
IZ_IMP char *malloc();
IZ_IMP char *getenv();
IZ_IMP long atol();
#endif /* NO_STDLIB_H */

#ifndef NO_MKTEMP
IZ_IMP char *mktemp();
#endif /* !NO_MKTEMP */

#endif /* NO_PROTO */

/*
 * SEEK_* macros, should be defined in stdio.h
 */
/* Define fseek() commands */
#ifndef SEEK_SET
#  define SEEK_SET 0
#endif /* !SEEK_SET */

#ifndef SEEK_CUR
#  define SEEK_CUR 1
#endif /* !SEEK_CUR */

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef TRUE
#  define TRUE 1
#endif

#ifdef NO_SIZE_T
   typedef unsigned int extent;
   /* define size_t 3/17/05 EG */
   typedef unsigned int size_t;
#else
   typedef size_t extent;
#endif

#ifdef NO_TIME_T
   typedef long time_t;
#endif

/* DBCS support for Info-ZIP's zip  (mainly for japanese (-: )
 * by Yoshioka Tsuneo (QWF00133@nifty.ne.jp,tsuneo-y@is.aist-nara.ac.jp)
 * This code is public domain!   Date: 1998/12/20
 */

/* 2007-07-29 SMS.
 * Include <locale.h> here if it will be needed later for Unicode.
 * Otherwise, SETLOCALE may be defined here, and then defined again
 * (differently) when <locale.h> is read later.
 */
#ifdef UNICODE_SUPPORT
# if defined( UNIX) || defined( VMS)
#   include <locale.h>
# endif /* defined( UNIX) || defined( VMS) */
# include <wchar.h>
# include <wctype.h>
#endif /* def UNICODE_SUPPORT */

#ifdef _MBCS
#   include <locale.h>

    /* Multi Byte Character Set */
    extern char *___tmp_ptr;
    unsigned char *zmbschr OF((ZCONST unsigned char *, unsigned int));
    unsigned char *zmbsrchr OF((ZCONST unsigned char *, unsigned int));
#   define CLEN(ptr) mblen((ZCONST char *)ptr, MB_CUR_MAX)
#   define PREINCSTR(ptr) (ptr += CLEN(ptr))
#   define POSTINCSTR(ptr) (___tmp_ptr=(char *)ptr,ptr += CLEN(ptr),___tmp_ptr)
    int lastchar OF((ZCONST char *ptr));
#   define MBSCHR(str,c) (char *)zmbschr((ZCONST unsigned char *)(str), c)
#   define MBSRCHR(str,c) (char *)zmbsrchr((ZCONST unsigned char *)(str), (c))
#   ifndef SETLOCALE
#      define SETLOCALE(category, locale) setlocale(category, locale)
#   endif /* ndef SETLOCALE */
#else /* !_MBCS */
#   define CLEN(ptr) 1
#   define PREINCSTR(ptr) (++(ptr))
#   define POSTINCSTR(ptr) ((ptr)++)
#   define lastchar(ptr) ((*(ptr)=='\0') ? '\0' : ptr[strlen(ptr)-1])
#   define MBSCHR(str, c) strchr(str, c)
#   define MBSRCHR(str, c) strrchr(str, c)
#   ifndef SETLOCALE
#      define SETLOCALE(category, locale)
#   endif /* ndef SETLOCALE */
#endif /* ?_MBCS */
#define INCSTR(ptr) PREINCSTR(ptr)


/* System independent replacement for "struct utimbuf", which is missing
 * in many older OS environments.
 */
typedef struct ztimbuf {
    time_t actime;              /* new access time */
    time_t modtime;             /* new modification time */
} ztimbuf;

/* This macro round a time_t value to the OS specific resolution */
#ifndef ROUNDED_TIME
#  define ROUNDED_TIME(time)   (time)
#endif

/* Some systems define S_IFLNK but do not support symbolic links */
#if defined (S_IFLNK) && defined(NO_SYMLINKS)
#  undef S_IFLNK
#endif

#ifndef Z_UINT4_DEFINED
#  if !defined(NO_LIMITS_H)
#    if (defined(UINT_MAX) && (UINT_MAX == 0xffffffffUL))
       typedef unsigned int     z_uint4;
#      define Z_UINT4_DEFINED
#    else
#      if (defined(ULONG_MAX) && (ULONG_MAX == 0xffffffffUL))
         typedef unsigned long    z_uint4;
#        define Z_UINT4_DEFINED
#      else
#        if (defined(USHRT_MAX) && (USHRT_MAX == 0xffffffffUL))
           typedef unsigned short   z_uint4;
#          define Z_UINT4_DEFINED
#        endif
#      endif
#    endif
#  endif /* !defined(NO_LIMITS_H) */
#endif /* ndef Z_UINT4_DEFINED */
#ifndef Z_UINT4_DEFINED
  typedef ulg                z_uint4;
# define Z_UINT4_DEFINED
#endif

#ifndef FOPR    /* fallback default definitions for FOPR, FOPM, FOPW: */
#  define FOPR "r"
#  define FOPM "r+"
#  define FOPW "w"
#endif /* fallback definition */

#ifndef FOPW_TMP    /* fallback default for opening writable temp files */
#  define FOPW_TMP FOPW
#endif

/* Open the old zip file in exclusive mode if possible (to avoid adding
 * zip file to itself).
 */
#ifdef OS2
#  define FOPR_EX FOPM
#else
#  define FOPR_EX FOPR
#endif


/* MSDOS file or directory attributes */
#define MSDOS_HIDDEN_ATTR 0x02
#define MSDOS_DIR_ATTR 0x10


/* Define this symbol if your target allows access to unaligned data.
 * This is not mandatory, just a speed optimization. The compressed
 * output is strictly identical.
 */
#if (defined(MSDOS) && !defined(WIN32)) || defined(i386)
#    define UNALIGNED_OK
#endif
#if defined(mc68020) || defined(vax)
#    define UNALIGNED_OK
#endif

#if (defined(SMALL_MEM) && !defined(CBSZ))
#   define CBSZ 2048 /* buffer size for copying files */
#   define ZBSZ 2048 /* buffer size for temporary zip file */
#endif

#if (defined(MEDIUM_MEM) && !defined(CBSZ))
#  define CBSZ 8192
#  define ZBSZ 8192
#endif

#ifndef CBSZ
#  define CBSZ 16384
#  define ZBSZ 16384
#endif

#ifndef SBSZ
#  define SBSZ CBSZ     /* copy buf size for STORED entries, see zipup() */
#endif

#ifndef MEMORY16
#  ifdef __WATCOMC__
#    undef huge
#    undef far
#    undef near
#  endif
#  ifdef THEOS
#    undef far
#    undef near
#  endif
#  if (!defined(__IBMC__) || !defined(OS2))
#    ifndef huge
#      define huge
#    endif
#    ifndef far
#      define far
#    endif
#    ifndef near
#      define near
#    endif
#  endif
#  define nearmalloc malloc
#  define nearfree free
#  define farmalloc malloc
#  define farfree free
#endif /* !MEMORY16 */

#ifndef Far
#  define Far far
#endif

/* MMAP and BIG_MEM cannot be used together -> let MMAP take precedence */
#if (defined(MMAP) && defined(BIG_MEM))
#  undef BIG_MEM
#endif

#if (defined(BIG_MEM) || defined(MMAP)) && !defined(DYN_ALLOC)
#   define DYN_ALLOC
#endif


/* LARGE_FILE_SUPPORT
 *
 * Types are in osdep.h for each port
 *
 * LARGE_FILE_SUPPORT and ZIP64_SUPPORT are automatically
 * set in osdep.h (for some ports) based on the port and compiler.
 *
 * Function prototypes are below as OF is defined earlier in this file
 * but after osdep.h is included.  In the future ANSI prototype
 * support may be required and the OF define may then go away allowing
 * the function defines to be in the port osdep.h.
 *
 * E. Gordon 9/21/2003
 * Updated 7/24/04 EG
 */
#ifdef LARGE_FILE_SUPPORT
  /* 64-bit Large File Support */

  /* Arguments for all functions are assumed to match the actual
     arguments of the various port calls.  As such only the
     function names are mapped below. */

/* ---------------------------- */
# ifdef UNIX

  /* Assume 64-bit file environment is defined.  The below should all
     be set to their 64-bit versions automatically.  Neat.  7/20/2004 EG */

    /* 64-bit stat functions */
#   define zstat stat
#   define zfstat fstat
#   define zlstat lstat

# if defined(__alpha) && defined(__osf__)  /* support for osf4.0f */
    /* 64-bit fseek */
#   define zfseeko fseek

    /* 64-bit ftell */
#   define zftello ftell

# else
     /* 64-bit fseeko */
#   define zfseeko fseeko

     /* 64-bit ftello */
#   define zftello ftello
# endif                                    /* __alpha && __osf__ */

    /* 64-bit fopen */
#   define zfopen fopen
#   define zfdopen fdopen

# endif /* UNIX */

/* ---------------------------- */
# ifdef VMS

    /* 64-bit stat functions */
#   define zstat stat
#   define zfstat fstat
#   define zlstat lstat

    /* 64-bit fseeko */
#   define zfseeko fseeko

    /* 64-bit ftello */
#   define zftello ftello

    /* 64-bit fopen */
#   define zfopen fopen
#   define zfdopen fdopen

# endif /* def VMS */

/* ---------------------------- */
# ifdef WIN32

#   if defined(__MINGW32__)
    /* GNU C, linked against "msvcrt.dll" */

      /* 64-bit stat functions */
#     define zstat _stati64
# ifdef UNICODE_SUPPORT
#     define zwfstat _fstati64
#     define zwstat _wstati64
#     define zw_stat struct _stati64
# endif
#     define zfstat _fstati64
#     define zlstat lstat

      /* 64-bit fseeko */
      /* function in win32.c */
      int zfseeko OF((FILE *, zoff_t, int));

      /* 64-bit ftello */
      /* function in win32.c */
      zoff_t zftello OF((FILE *));

      /* 64-bit fopen */
#     define zfopen fopen
#     define zfdopen fdopen

#   endif

#   if defined(__CYGWIN__)
    /* GNU C, CygWin with its own POSIX compatible runtime library */

      /* 64-bit stat functions */
#     define zstat stat
#     define zfstat fstat
#     define zlstat lstat

      /* 64-bit fseeko */
#     define zfseeko fseeko

      /* 64-bit ftello */
#     define zftello ftello

      /* 64-bit fopen */
#     define zfopen fopen
#     define zfdopen fdopen

#   endif

#   ifdef __WATCOMC__
    /* WATCOM C */

      /* 64-bit stat functions */
#     define zstat _stati64
# ifdef UNICODE_SUPPORT
#     define zwfstat _fstati64
#     define zwstat _wstati64
#     define zw_stat struct _stati64
# endif
#     define zfstat _fstati64
#     define zlstat lstat

      /* 64-bit fseeko */
      /* function in win32.c */
      int zfseeko OF((FILE *, zoff_t, int));

      /* 64-bit ftello */
      /* function in win32.c */
      zoff_t zftello OF((FILE *));

      /* 64-bit fopen */
#     define zfopen fopen
#     define zfdopen fdopen

#   endif

#   ifdef _MSC_VER
    /* MS C and VC */

      /* 64-bit stat functions */
#     define zstat _stati64
# ifdef UNICODE_SUPPORT
#     define zwfstat _fstati64
#     define zwstat _wstati64
#     define zw_stat struct _stati64
# endif
#     define zfstat _fstati64
#     define zlstat lstat

      /* 64-bit fseeko */
      /* function in win32.c */
      int zfseeko OF((FILE *, zoff_t, int));

      /* 64-bit ftello */
      /* function in win32.c */
      zoff_t zftello OF((FILE *));

      /* 64-bit fopen */
#     define zfopen fopen
#     define zfdopen fdopen

#   endif

#   ifdef __IBMC__
      /* IBM C */

      /* 64-bit stat functions */

      /* 64-bit fseeko */
      /* function in win32.c */
      int zfseeko OF((FILE *, zoff_t, int));

      /* 64-bit ftello */
      /* function in win32.c */
      zoff_t zftello OF((FILE *));

      /* 64-bit fopen */

#   endif

# endif /* WIN32 */

#else
  /* No Large File Support or default for 64-bit environment */

# define zstat stat
# define zfstat fstat
# define zlstat lstat
# define zfseeko fseek
# define zftello ftell
# define zfopen fopen
# define zfdopen fdopen
# ifdef UNICODE_SUPPORT
#   define zwfstat _fstat
#   define zwstat _wstat
#   define zw_stat struct _stat
# endif

#endif

#ifdef LARGE_FILE_SUPPORT         /* E. Gordon 9/12/2003 */

# ifndef SSTAT
#  define SSTAT      zstat
#  ifdef UNICODE_SUPPORT
#    define SSTATW   zwstat
#  endif
# endif
# ifdef S_IFLNK
#  define LSTAT      zlstat
#  define LSSTAT(n, s)  (linkput ? zlstat((n), (s)) : SSTAT((n), (s)))
# else
#  define LSTAT      SSTAT
#  define LSSTAT     SSTAT
#  ifdef UNICODE_SUPPORT
#    define LSSTATW  SSTATW
#  endif
# endif

#else /* no LARGE_FILE_SUPPORT */

# ifndef SSTAT
#  define SSTAT      stat
# endif
# ifdef S_IFLNK
#  define LSTAT      lstat
#  define LSSTAT(n, s)  (linkput ? lstat((n), (s)) : SSTAT((n), (s)))
# else
#  define LSTAT      SSTAT
#  define LSSTAT     SSTAT
#  ifdef UNICODE_SUPPORT
#    define LSSTATW  SSTATW
#  endif
# endif

#endif


/*---------------------------------------------------------------------*/


/* 2004-12-01 SMS.
 * Added fancy zofft() macros, et c.
 */

/* Default fzofft() format selection.
 * Modified 2004-12-27 EG
 */

#ifndef FZOFFT_FMT
# define FZOFFT_FMT      ZOFF_T_FORMAT_SIZE_PREFIX /* printf for zoff_t values */

# ifdef LARGE_FILE_SUPPORT
#   define FZOFFT_HEX_WID_VALUE     "16"  /* width of 64-bit hex values */
# else
#   define FZOFFT_HEX_WID_VALUE     "8"   /* digits in 32-bit hex values */
# endif

#endif /* ndef FZOFFT_FMT */

#define FZOFFT_HEX_WID ((char *) -1)
#define FZOFFT_HEX_DOT_WID ((char *) -2)




/* The following default definition of the second input for the crypthead()
 * random seed computation can be used on most systems (all those that
 * supply a UNIX compatible getpid() function).
 */
#ifdef ZCRYPT_INTERNAL
#  ifndef ZCR_SEED2
#    define ZCR_SEED2     (unsigned) getpid()   /* use PID as seed pattern */
#  endif
#endif /* ZCRYPT_INTERNAL */

/* The following OS codes are defined in pkzip appnote.txt */
#ifdef AMIGA
#  define OS_CODE  0x100
#endif
#ifdef VMS
#  define OS_CODE  0x200
#endif
/* unix    3 */
#ifdef VM_CMS
#  define OS_CODE  0x400
#endif
#ifdef ATARI
#  define OS_CODE  0x500
#endif
#ifdef OS2
#  define OS_CODE  0x600
#endif
#ifdef MACOS
#  define OS_CODE  0x700
#endif
/* z system 8 */
/* cp/m     9 */
#ifdef TOPS20
#  define OS_CODE  0xa00
#endif
#ifdef WIN32
#  define OS_CODE  0xb00
#endif
#ifdef QDOS
#  define OS_CODE  0xc00
#endif
#ifdef RISCOS
#  define OS_CODE  0xd00
#endif
#ifdef VFAT
#  define OS_CODE  0xe00
#endif
#ifdef MVS
#  define OS_CODE  0xf00
#endif
#ifdef __BEOS__
#  define OS_CODE  0x1000
#endif
#ifdef TANDEM
#  define OS_CODE  0x1100
#endif
#ifdef THEOS
#  define OS_CODE  0x1200
#endif
/* Yes, there is a gap here. */
#ifdef __ATHEOS__
#  define OS_CODE  0x1E00
#endif

#define NUM_HOSTS 31
/* Number of operating systems. Should be updated when new ports are made */

#if defined(DOS) && !defined(OS_CODE)
#  define OS_CODE  0x000
#endif

#ifndef OS_CODE
#  define OS_CODE  0x300  /* assume Unix */
#endif

/* can't use "return 0" from main() on VMS */
#ifndef EXIT
#  define EXIT  exit
#endif
#ifndef RETURN
#  define RETURN return
#endif

#ifndef ZIPERR
#  define ZIPERR ziperr
#endif

#if (defined(USE_ZLIB) && defined(MY_ZCALLOC))
   /* special zcalloc function is not needed when linked against zlib */
#  undef MY_ZCALLOC
#endif

#if (!defined(USE_ZLIB) && !defined(MY_ZCALLOC))
   /* Any system without a special calloc function */
#  define zcalloc(items,size) \
          (zvoid far *)calloc((unsigned)(items), (unsigned)(size))
#  define zcfree    free
#endif /* !USE_ZLIB && !MY_ZCALLOC */

/* end of tailor.h */
