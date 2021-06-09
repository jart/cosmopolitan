/*
  win32/osdep.h

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

/* Automatic setting of the common Microsoft C idenfifier MSC.
 * NOTE: Watcom also defines M_I*86 !
 */
#if defined(_MSC_VER) || (defined(M_I86) && !defined(__WATCOMC__))
#  ifndef MSC
#    define MSC                 /* This should work for older MSC, too!  */
#  endif
#endif

/* Tell Microsoft Visual C++ 2005 to leave us alone and
 * let us use standard C functions the way we're supposed to.
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  ifndef _CRT_SECURE_NO_DEPRECATE
#    define _CRT_SECURE_NO_DEPRECATE
#  endif
#  ifndef _CRT_NONSTDC_NO_DEPRECATE
#    define _CRT_NONSTDC_NO_DEPRECATE
#  endif
#endif

#if defined(__WATCOMC__) && defined(__386__)
#  define WATCOMC_386
#endif

#if (defined(__CYGWIN32__) && !defined(__CYGWIN__))
#  define __CYGWIN__            /* compatibility for CygWin B19 and older */
#endif

/* enable multibyte character set support by default */
#ifndef _MBCS
#  define _MBCS
#endif
#if defined(__CYGWIN__)
#  undef _MBCS
#endif

/* Get types and stat */
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#ifndef MSDOS
/*
 * Windows 95 (and Windows NT) file systems are (to some extend)
 * extensions of MSDOS. Common features include for example:
 *      FAT or (FAT like) file systems,
 *      '\\' as directory separator in paths,
 *      "\r\n" as record (line) terminator in text files, ...
 */
#  define MSDOS
/* inherit MS-DOS file system etc. stuff */
#endif

#define USE_CASE_MAP
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : \
                     procname(n, filter_match_case))
#define BROKEN_FSEEK
#ifndef __RSXNT__
#  define HAVE_FSEEKABLE
#endif


/* popen
 *
 * On Win32 must map to _popen() and _pclose()
 */
#define popen _popen
#define pclose _pclose

/* WIN32_OEM
 *
 * This enables storing paths in archives on WIN32 in OEM format
 * which is more work but seems the standard now.  It also enables
 * converting paths in read DOS archives from assumed OEM to ANSI.
 */
#ifndef NO_WIN32_OEM
#  define WIN32_OEM
#endif

/* Large File Support
 *
 *  If this is set it is assumed that the port
 *  supports 64-bit file calls.  The types are
 *  defined here.  Any local implementations are
 *  in Win32.c and the prototypes for the calls are
 *  in tailor.h.  Note that a port must support
 *  these calls fully or should not set
 *  LARGE_FILE_SUPPORT.
 */

/* Note also that ZOFF_T_FORMAT_SIZE_PREFIX has to be defined here
   or tailor.h will define defaults */

/* If port has LARGE_FILE_SUPPORT then define here
   to make large file support automatic unless overridden */


#ifndef LARGE_FILE_SUPPORT
# ifndef NO_LARGE_FILE_SUPPORT
    /* MS C and VC */
#   if defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__)
#     define LARGE_FILE_SUPPORT
#   endif
#   if defined(__WATCOMC__)
#     define LARGE_FILE_SUPPORT
#   endif
# endif
#endif

#ifdef LARGE_FILE_SUPPORT
  /* 64-bit Large File Support */

  /* Only types and the printf format stuff go here.  Functions
     go in tailor.h since ANSI prototypes are required and the OF define
     is not defined here. */

# if (defined(_MSC_VER) && (_MSC_VER >= 1100)) || defined(__MINGW32__)
    /* MS C and VC, MinGW32 */
    /* these compiler systems use the Microsoft C RTL */

    /* base types for file offsets and file sizes */
    typedef __int64             zoff_t;
    typedef unsigned __int64    uzoff_t;

    /* 64-bit stat struct */
    typedef struct _stati64 z_stat;

    /* printf format size prefix for zoff_t values */
#   define ZOFF_T_FORMAT_SIZE_PREFIX "I64"

# elif (defined(__GNUC__) || defined(ULONG_LONG_MAX))
    /* GNU C */

    /* base types for file offsets and file sizes */
    typedef long long           zoff_t;
    typedef unsigned long long  uzoff_t;

#  ifdef __CYGWIN__
    /* Use Cygwin's own stat struct */
     typedef struct stat z_stat;
#  else
    /* 64-bit stat struct */
    typedef struct _stati64 z_stat;
#  endif

    /* printf format size prefix for zoff_t values */
#   define ZOFF_T_FORMAT_SIZE_PREFIX "ll"

# elif (defined(__WATCOMC__) && (__WATCOMC__ >= 1100))
    /* WATCOM C */

    /* base types for file offsets and file sizes */
    typedef __int64             zoff_t;
    typedef unsigned __int64    uzoff_t;

    /* 64-bit stat struct */
    typedef struct _stati64 z_stat;

    /* printf format size prefix for zoff_t values */
#   define ZOFF_T_FORMAT_SIZE_PREFIX "ll"

# elif (defined(__IBMC__) && (__IBMC__ >= 350))
    /* IBM C */

    /* base types for file offsets and file sizes */
    typedef __int64             zoff_t;
    typedef unsigned __int64    uzoff_t;

    /* 64-bit stat struct */

    /* printf format size prefix for zoff_t values */
#   define ZOFF_T_FORMAT_SIZE_PREFIX "I64"

# else
#   undef LARGE_FILE_SUPPORT
# endif

#endif

#if 0
# ifndef ZOFF_T_FORMAT_SIZE_PREFIX
    /* unsupported WIN32 */

    /* base types for file offsets and file sizes */
    typedef long long           zoff_t;
    typedef unsigned long long  uzoff_t;

    /* 64-bit stat struct */
    typedef struct stat z_stat;

    /* printf format size prefix for zoff_t values */
#   define ZOFF_T_FORMAT_SIZE_PREFIX "ll"
# endif
#endif


/* Automatically set ZIP64_SUPPORT if supported */

/* MS C and VC */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__WATCOMC__)
# ifdef LARGE_FILE_SUPPORT
#   ifndef NO_ZIP64_SUPPORT
#     ifndef ZIP64_SUPPORT
#       define ZIP64_SUPPORT
#     endif
#   endif
# endif
#endif


#ifndef LARGE_FILE_SUPPORT
  /* No Large File Support */

  /* base type for file offsets and file sizes */
  typedef long zoff_t;
  typedef unsigned long uzoff_t;

  /* stat struct */
  typedef struct stat z_stat;

  /* printf format size prefix for zoff_t values */
# define ZOFF_T_FORMAT_SIZE_PREFIX "l"
#endif


  /* UNICODE */
#ifdef WIN32
  /* assume wide character conversion functions */
# ifndef UNICODE_SUPPORT
#   ifndef NO_UNICODE_SUPPORT
#     define UNICODE_SUPPORT
#   endif
# endif
#endif

#if 0
  /* this is now generic */
# ifdef UNICODE_SUPPORT
  /* Set up Unicode support - 9/27/05 EG */

  /* type of wide string characters */
#  define zchar wchar_t

  /* default char string used if a wide char can't be converted */
#  define zchar_default "_"

# else
#  define zchar char
# endif
#endif


/* File operations--use "b" for binary if allowed or fixed length 512 on VMS
 *                  use "S" for sequential access on NT to prevent the NT
 *                  file cache eating up memory with large .zip files
 */
#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "wbS"

#if (defined(__CYGWIN__) && !defined(NO_MKTIME))
#  define NO_MKTIME             /* Cygnus' mktime() implementation is buggy */
#endif
#if (!defined(NT_TZBUG_WORKAROUND) && !defined(NO_NT_TZBUG_WORKAROUND))
#  define NT_TZBUG_WORKAROUND
#endif
#if (defined(UTIL) && defined(NT_TZBUG_WORKAROUND))
#  undef NT_TZBUG_WORKAROUND    /* the Zip utilities do not use time-stamps */
#endif
#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  define USE_EF_UT_TIME
#endif
#if (!defined(NO_NTSD_EAS) && !defined(NTSD_EAS))
#  define NTSD_EAS
#endif

#if (defined(NTSD_EAS) && !defined(ZP_NEED_MEMCOMPR))
#  define ZP_NEED_MEMCOMPR
#endif

#ifdef WINDLL
# ifndef NO_ASM
#   define NO_ASM
# endif
# ifndef MSWIN
#   define MSWIN
# endif
# ifndef REENTRANT
#   define REENTRANT
# endif
#endif /* WINDLL */

/* Enable use of optimized x86 assembler version of longest_match() for
   MSDOS, WIN32 and OS2 per default.  */
#if !defined(NO_ASM) && !defined(ASMV)
#  define ASMV
#endif

/* Enable use of optimized x86 assembler version of crc32() for
   MSDOS, WIN32 and OS2 per default.  */
#if !defined(NO_ASM) && !defined(ASM_CRC)  && !defined(NO_ASM_CRC)
#  define ASM_CRC
#endif

#if !defined(__GO32__) && !defined(__EMX__) && !defined(__CYGWIN__)
#  define NO_UNISTD_H
#endif

/* Microsoft C requires additional attributes attached to all RTL function
 * declarations when linking against the CRTL dll.
 */
#ifdef MSC
#  ifdef IZ_IMP
#    undef IZ_IMP
#  endif
#  define IZ_IMP _CRTIMP
#else
# ifndef IZ_IMP
#   define IZ_IMP
# endif
#endif

/* WIN32 runs solely on little-endian processors; enable support
 * for the 32-bit optimized CRC-32 C code by default.
 */
#ifdef IZ_CRC_BE_OPTIMIZ
#  undef IZ_CRC_BE_OPTIMIZ
#endif
#if !defined(IZ_CRC_LE_OPTIMIZ) && !defined(NO_CRC_OPTIMIZ)
#  define IZ_CRC_LE_OPTIMIZ
#endif

/* the following definitions are considered as "obsolete" by Microsoft and
 * might be missing in some versions of <windows.h>
 */
#ifndef AnsiToOem
#  define AnsiToOem CharToOemA
#endif
#ifndef OemToAnsi
#  define OemToAnsi OemToCharA
#endif

/* handlers for OEM <--> ANSI string conversions */
#if defined(__RSXNT__) || defined(WIN32_CRT_OEM)
   /* RSXNT uses OEM coded strings in functions supplied by C RTL */
#  ifdef CRTL_CP_IS_ISO
#    undef CRTL_CP_IS_ISO
#  endif
#  ifndef CRTL_CP_IS_OEM
#    define CRTL_CP_IS_OEM
#  endif
#else
   /* "real" native WIN32 compilers use ANSI coded strings in C RTL calls */
#  ifndef CRTL_CP_IS_ISO
#    define CRTL_CP_IS_ISO
#  endif
#  ifdef CRTL_CP_IS_OEM
#    undef CRTL_CP_IS_OEM
#  endif
#endif

#ifdef CRTL_CP_IS_ISO
   /* C RTL's file system support assumes ANSI coded strings */
#  define ISO_TO_INTERN(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define OEM_TO_INTERN(src, dst)  OemToAnsi(src, dst)
#  define INTERN_TO_ISO(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define INTERN_TO_OEM(src, dst)  AnsiToOem(src, dst)
#  define _OEM_INTERN(str1) OEM_TO_INTERN(str1, str1)
#  define _ISO_INTERN(str1) {;}
#  define _INTERN_OEM(str1) INTERN_TO_OEM(str1, str1)
#  define _INTERN_ISO(str1) {;}
#endif /* CRTL_CP_IS_ISO */
#ifdef CRTL_CP_IS_OEM
   /* C RTL's file system support assumes OEM coded strings */
#  define ISO_TO_INTERN(src, dst)  AnsiToOem(src, dst)
#  define OEM_TO_INTERN(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define INTERN_TO_ISO(src, dst)  OemToAnsi(src, dst)
#  define INTERN_TO_OEM(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define _OEM_INTERN(str1) {;}
#  define _ISO_INTERN(str1) ISO_TO_INTERN(str1, str1)
#  define _INTERN_OEM(str1) {;}
#  define _INTERN_ISO(str1) INTERN_TO_ISO(str1, str1)
#endif /* CRTL_CP_IS_OEM */

/* The following "OEM vs. ISO Zip entry names" code has been copied from UnZip.
 * It should be applicable to the generic Zip code. However, currently only
 * the Win32 port of Zip supplies the required charset conversion functions.
 * (The Win32 port uses conversion functions supplied by the OS.)
 */
/* Convert filename (and file comment string) into "internal" charset.
 * This macro assumes that Zip entry filenames are coded in OEM (IBM DOS)
 * codepage when made on
 *  -> DOS (this includes 16-bit Windows 3.1)  (FS_FAT_)
 *  -> OS/2                                    (FS_HPFS_)
 *  -> Win95/WinNT with Nico Mak's WinZip      (FS_NTFS_ && hostver == "5.0")
 * EXCEPTIONS:
 *  PKZIP for Windows 2.5, 2.6, and 4.0 flag their entries as "FS_FAT_", but
 *  the filename stored in the local header is coded in Windows ANSI (CP 1252
 *  resp. ISO 8859-1 on US and western Europe locale settings).
 *  Likewise, PKZIP for UNIX 2.51 flags its entries as "FS_FAT_", but the
 *  filenames stored in BOTH the local and the central header are coded
 *  in the local system's codepage (usually ANSI codings like ISO 8859-1,
 *  but could also be UTF-8 on "modern" setups...).
 *
 * All other ports are assumed to code zip entry filenames in ISO (8859-1
 * on "Western" localisations).
 */
#define FS_FAT_           0    /* filesystem used by MS-DOS, OS/2, Win32 */
#define FS_HPFS_          6    /* filesystem used by OS/2 (and NT 3.x) */
#define FS_NTFS_          11   /* filesystem used by Windows NT */
#ifndef Ext_ASCII_TO_Native
#  define Ext_ASCII_TO_Native(string, hostnum, hostver, isuxatt, islochdr) \
    if (((hostnum) == FS_FAT_ && \
         !(((islochdr) || (isuxatt)) && \
           ((hostver) == 25 || (hostver) == 26 || (hostver) == 40))) || \
        (hostnum) == FS_HPFS_ || \
        ((hostnum) == FS_NTFS_ && (hostver) == 50)) { \
        _OEM_INTERN((string)); \
    } else { \
        _ISO_INTERN((string)); \
    }
#endif

#if (defined(__RSXNT__) && defined(__CRTRSXNT__))
#  include <crtrsxnt.h>
#endif

#ifdef _MBCS
#  if (!defined(__EMX__) && !defined(__MINGW32__) && !defined(__CYGWIN__))
#    include <stdlib.h>
#    include <mbstring.h>
#  endif
#  if (defined(__MINGW32__) && !defined(MB_CUR_MAX))
#    ifdef __MSVCRT__
       IZ_IMP extern int *__p___mb_cur_max(void);
#      define MB_CUR_MAX (*__p___mb_cur_max())
#    else
       IZ_IMP extern int *_imp____mb_cur_max_dll;
#      define MB_CUR_MAX (*_imp____mb_cur_max_dll)
#    endif
#  endif
#  if (defined(__LCC__) && !defined(MB_CUR_MAX))
     IZ_IMP extern int *_imp____mb_cur_max;
#    define MB_CUR_MAX (*_imp____mb_cur_max)
#  endif
#endif

#ifdef __LCC__
#  include <time.h>
#  ifndef tzset
#    define tzset _tzset
#  endif
#  ifndef utime
#    define utime _utime
#  endif
#endif
#ifdef __MINGW32__
   IZ_IMP extern void _tzset(void);     /* this is missing in <time.h> */
#  ifndef tzset
#    define tzset _tzset
#  endif
#endif
#if (defined(__RSXNT__) || defined(__EMX__)) && !defined(tzset)
#  define tzset _tzset
#endif
#ifdef W32_USE_IZ_TIMEZONE
#  ifdef __BORLANDC__
#    define tzname tzname
#    define IZTZ_DEFINESTDGLOBALS
#  endif
#  ifndef tzset
#    define tzset _tzset
#  endif
#  ifndef timezone
#    define timezone _timezone
#  endif
#  ifndef daylight
#    define daylight _daylight
#  endif
#  ifndef tzname
#    define tzname _tzname
#  endif
#  if (!defined(NEED__ISINDST) && !defined(__BORLANDC__))
#    define NEED__ISINDST
#  endif
#  ifdef IZTZ_GETLOCALETZINFO
#    undef IZTZ_GETLOCALETZINFO
#  endif
#  define IZTZ_GETLOCALETZINFO GetPlatformLocalTimezone
#endif /* W32_USE_IZ_TIMEZONE */

#ifdef MATCH
#  undef MATCH
#endif
#define MATCH dosmatch          /* use DOS style wildcard matching */
#ifdef UNICODE_SUPPORT
# ifdef WIN32
#   define MATCHW dosmatchw
# endif
#endif

#ifdef ZCRYPT_INTERNAL
#  ifdef WINDLL
#    define ZCR_SEED2     (unsigned)3141592654L /* use PI as seed pattern */
#  else
#    include <process.h>        /* getpid() declaration for srand seed */
#  endif
#endif

/* Up to now, all versions of Microsoft C runtime libraries lack the support
 * for customized (non-US) switching rules between daylight saving time and
 * standard time in the TZ environment variable string.
 * But non-US timezone rules are correctly supported when timezone information
 * is read from the OS system settings in the Win32 registry.
 * The following work-around deletes any TZ environment setting from
 * the process environment.  This results in a fallback of the RTL time
 * handling code to the (correctly interpretable) OS system settings, read
 * from the registry.
 */
#ifdef USE_EF_UT_TIME
# if (defined(__WATCOMC__) || defined(__CYGWIN__) || \
      defined(W32_USE_IZ_TIMEZONE))
#   define iz_w32_prepareTZenv()
# else
#   define iz_w32_prepareTZenv()        putenv("TZ=")
# endif
#endif

/* This patch of stat() is useful for at least three compilers.  It is   */
/* difficult to take a stat() of a root directory under Windows95, so  */
/* zstat_zipwin32() detects that case and fills in suitable values.    */
#ifndef __RSXNT__
#  ifndef W32_STATROOT_FIX
#    define W32_STATROOT_FIX
#  endif
#endif /* !__RSXNT__ */

#if (defined(NT_TZBUG_WORKAROUND) || defined(W32_STATROOT_FIX))
#  define W32_STAT_BANDAID
#  ifdef LARGE_FILE_SUPPORT         /* E. Gordon 9/12/03 */
   int zstat_zipwin32(const char *path, z_stat *buf);
#  else
   int zstat_zipwin32(const char *path, struct stat *buf);
#  endif
#  ifdef UNICODE_SUPPORT
#   ifdef LARGE_FILE_SUPPORT
     int zstat_zipwin32w(const wchar_t *pathw, struct _stati64 *buf);
#   else
     int zstat_zipwin32w(const wchar_t *pathw, struct _stat *buf);
#   endif
#  endif
#  ifdef SSTAT
#    undef SSTAT
#  endif
#  define SSTAT zstat_zipwin32
#  ifdef UNICODE_SUPPORT
#    define SSTATW zstat_zipwin32w
#  endif
#endif /* NT_TZBUG_WORKAROUND || W32_STATROOT_FIX */

int getch_win32(void);

#ifdef __GNUC__
# define IZ_PACKED      __attribute__((packed))
#else
# define IZ_PACKED
#endif

/* for some (all ?) versions of IBM C Set/2 and IBM C Set++ */
#ifndef S_IFMT
#  define S_IFMT 0xF000
#endif /* !S_IFMT */

#ifdef __WATCOMC__
#  include <stdio.h>    /* PATH_MAX is defined here */
#  define NO_MKTEMP

/* Get asm routines to link properly without using "__cdecl": */
#  ifdef __386__
#    ifdef ASMV
#      pragma aux match_init    "_*" parm caller [] modify []
#      pragma aux longest_match "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif
#    if defined(ASM_CRC) && !defined(USE_ZLIB)
#      pragma aux crc32         "_*" parm caller [] value [eax] modify [eax]
#      pragma aux get_crc_table "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif /* ASM_CRC && !USE_ZLIB */
#  endif /* __386__ */
   /* Watcom C (like the other Win32 C compiler systems) does not support
    * symlinks on Win32, but defines the S_IFLNK symbol nevertheless.
    * However, the existence of this symbol is used as "symlinks supported"
    * indicator in the generic Zip code (see tailor.h). So, for a simple
    * work-around, this symbol is undefined here. */
#  ifdef S_IFLNK
#    undef S_IFLNK
#  endif
#  ifdef UNICODE_SUPPORT
     /* Watcom C does not supply wide-char definitions in the "standard"
      * headers like MSC; so we have to pull in a wchar-specific header.
      */
#    include <wchar.h>
#  endif
#endif /* __WATCOMC__ */
