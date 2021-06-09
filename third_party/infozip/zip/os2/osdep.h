/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#if defined(__OS2__) && !defined(OS2)
#  define OS2
#endif

/* Automatic setting of the common Microsoft C idenfifier MSC.
 * NOTE: Watcom also defines M_I*86 !
 */
#if defined(_MSC_VER) || (defined(M_I86) && !defined(__WATCOMC__))
#  ifndef MSC
#    define MSC                 /* This should work for older MSC, too!  */
#  endif
#endif

#if defined(__WATCOMC__) && defined(__386__)
#  define WATCOMC_386
#endif

#if defined(__EMX__) || defined(WATCOMC_386) || defined(__BORLANDC__)
#  if (defined(OS2) && !defined(__32BIT__))
#    define __32BIT__
#  endif
#endif

#if defined(OS2) && !defined(__32BIT__)
#  define MEMORY16
#endif

#ifndef NO_ASM
#  define ASMV
/* #  define ASM_CRC */
#endif

/* enable creation of UTC time fields unless explicitely suppressed */
#if (!defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME))
#  define USE_EF_UT_TIME
#endif

/* check that TZ environment variable is defined before using UTC times */
#if (!defined(NO_IZ_CHECK_TZ) && !defined(IZ_CHECK_TZ))
#  define IZ_CHECK_TZ
#endif

#ifndef ZP_NEED_MEMCOMPR
#  define ZP_NEED_MEMCOMPR
#endif

#ifdef MEMORY16
#  ifdef __TURBOC__
#    include <alloc.h>
#    if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
#      if defined(DYNAMIC_CRC_TABLE) && defined(DYNALLOC_CRCTAB)
        error: No dynamic CRC table allocation with Borland C far data models.
#      endif /* DYNAMIC_CRC_TABLE */
#    endif /* Turbo/Borland C far data memory models */
#    define nearmalloc malloc
#    define nearfree   free
#    define DYN_ALLOC
#  else /* !__TURBOC__ */
#    include <malloc.h>
#    define nearmalloc _nmalloc
#    define nearfree   _nfree
#    define farmalloc  _fmalloc
#    define farfree    _ffree
#  endif /* ?__TURBOC__ */
#  define MY_ZCALLOC 1
#endif /* MEMORY16 */


/* The symbol MSDOS is consistently used in the generic source files
 * to identify code to support for MSDOS (and MSDOS related) stuff.
 * e.g: FAT or (FAT like) file systems,
 *      '\\' as directory separator in paths,
 *      "\r\n" as record (line) terminator in text files, ...
 *
 * MSDOS is defined anyway with MS C 16-bit. So the block above works.
 * For the 32-bit compilers, MSDOS must not be defined in the block above.
 */
#if (defined(OS2) && !defined(MSDOS))
#  define MSDOS
/* inherit MS-DOS file system etc. stuff */
#endif

#define USE_CASE_MAP
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, 1))

/* time stamp resolution of file system is 2 seconds */
#define ROUNDED_TIME(time)  ((time_t)(((unsigned long)(time) + 1) & (~1)))

#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "wb"

#ifdef __32BIT__
#  define CBSZ 0x40000
#  define ZBSZ 0x40000
#else
#  define CBSZ 0xE000
#  define ZBSZ 0x7F00 /* Some libraries do not allow a buffer size > 32K */
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#ifdef ZCRYPT_INTERNAL
#  ifndef __GO32__
#    include <process.h>        /* getpid() declaration for srand seed */
#  endif
#endif

/* for some (all ?) versions of IBM C Set/2 and IBM C Set++ */
#ifndef S_IFMT
#  define S_IFMT 0xF000
#endif /* !S_IFMT */

#ifdef MSC
#  define NO_UNISTD_H
#endif

#ifdef __WATCOMC__
#  define NO_MKTEMP
/* Get asm routines to link properly without using "__cdecl": */
#  ifdef __386__
#    ifdef ASMV
#      pragma aux window "*";
#      pragma aux prev "*";
#      pragma aux prev_length "*";
#      pragma aux strstart "*";
#      pragma aux match_start "*";
#      pragma aux max_chain_length "*";
#      pragma aux good_match "*";
#      pragma aux nice_match "*";
#      pragma aux match_init "*";
#      pragma aux longest_match "*";
#    endif
#    ifndef USE_ZLIB
#      pragma aux crc32         "_*" parm caller [] value [eax] modify [eax]
#      pragma aux get_crc_table "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif /* !USE_ZLIB */
#  else /* !__386__ */
#    if defined(ASMV) || defined(ASM_CRC)
/*#      error 16 bit assembly modules currently DO NOT WORK with Watcom C. */
#    endif
#    ifdef ASMV
#      pragma aux match_init    "_*" parm caller [] loadds modify [ax bx]
#      pragma aux longest_match "_*" parm caller [] loadds value [ax] \
                                      modify [ax bx cx dx es]
#    endif
#    ifndef USE_ZLIB
#      pragma aux crc32         "_*" parm caller [] value [ax dx] \
                                      modify [ax bx cx dx es]
#      pragma aux get_crc_table "_*" parm caller [] value [ax] \
                                      modify [ax bx cx dx]
#    endif /* !USE_ZLIB */
#  endif /* ?__386__ */
#endif

#ifdef __IBMC__
#  define NO_UNISTD_H
#  define NO_MKTEMP
#  define timezone _timezone            /* (underscore names work with    */
#  define tzset _tzset                  /*  all versions of C Set)        */
#endif
