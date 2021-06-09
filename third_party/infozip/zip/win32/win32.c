/*
  win32/win32.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

/*
 * WIN32 specific functions for ZIP.
 *
 * The WIN32 version of ZIP heavily relies on the MSDOS and OS2 versions,
 * since we have to do similar things to switch between NTFS, HPFS and FAT.
 */


#include "../zip.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
/* for LARGE_FILE_SUPPORT but may not be needed */
#include <io.h>

#ifdef __RSXNT__
#  include <alloca.h>
#  include "../win32/rsxntwin.h"
#endif
#include "../win32/win32zip.h"

#define A_RONLY    0x01
#define A_HIDDEN   0x02
#define A_SYSTEM   0x04
#define A_LABEL    0x08
#define A_DIR      0x10
#define A_ARCHIVE  0x20


#define EAID     0x0009

#if (defined(__MINGW32__) && !defined(USE_MINGW_GLOBBING))
   int _CRT_glob = 0;   /* suppress command line globbing by C RTL */
#endif

#ifndef UTIL

extern int noisy;

#ifdef NT_TZBUG_WORKAROUND
local int FSusesLocalTime(const char *path);
#ifdef UNICODE_SUPPORt
local int FSusesLocalTimeW(const wchar_t *path);
#endif
#endif
#if (defined(USE_EF_UT_TIME) || defined(NT_TZBUG_WORKAROUND))
local int FileTime2utime(FILETIME *pft, time_t *ut);
#endif
#if (defined(NT_TZBUG_WORKAROUND) && defined(W32_STAT_BANDAID))
local int VFatFileTime2utime(const FILETIME *pft, time_t *ut);
#endif


/* FAT / HPFS detection */

int IsFileSystemOldFAT(char *dir)
{
  static char lastDrive = '\0';    /* cached drive of last GetVolumeInformation call */
  static int lastDriveOldFAT = 0;  /* cached OldFAT value of last GetVolumeInformation call */
  char root[4];
  DWORD vfnsize;
  DWORD vfsflags;

    /*
     * We separate FAT and HPFS+other file systems here.
     * I consider other systems to be similar to HPFS/NTFS, i.e.
     * support for long file names and being case sensitive to some extent.
     */

    strncpy(root, dir, 3);
    if ( isalpha((uch)root[0]) && (root[1] == ':') ) {
      root[0] = to_up(dir[0]);
      root[2] = '\\';
      root[3] = 0;
    }
    else {
      root[0] = '\\';
      root[1] = 0;
    }
    if (lastDrive == root[0]) {
      return lastDriveOldFAT;
    }

    if ( !GetVolumeInformation(root, NULL, 0,
                               NULL, &vfnsize, &vfsflags,
                               NULL, 0)) {
        fprintf(mesg, "zip diagnostic: GetVolumeInformation failed\n");
        return(FALSE);
    }

    lastDrive = root[0];
    lastDriveOldFAT = vfnsize <= 12;

    return lastDriveOldFAT;
}

#ifdef UNICODE_SUPPORT
int IsFileSystemOldFATW(wchar_t *dir)
{
  static wchar_t lastDrive = (wchar_t)'\0';    /* cached drive of last GetVolumeInformation call */
  static int lastDriveOldFAT = 0;  /* cached OldFAT value of last GetVolumeInformation call */
  wchar_t root[4];
  DWORD vfnsize;
  DWORD vfsflags;

    /*
     * We separate FAT and HPFS+other file systems here.
     * I consider other systems to be similar to HPFS/NTFS, i.e.
     * support for long file names and being case sensitive to some extent.
     */

    wcsncpy(root, dir, 3);
    if ( iswalpha(root[0]) && (root[1] == (wchar_t)':') ) {
      root[0] = towupper(dir[0]);
      root[2] = (wchar_t)'\\';
      root[3] = 0;
    }
    else {
      root[0] = (wchar_t)'\\';
      root[1] = 0;
    }
    if (lastDrive == root[0]) {
      return lastDriveOldFAT;
    }

    if ( !GetVolumeInformationW(root, NULL, 0,
                                NULL, &vfnsize, &vfsflags,
                                NULL, 0)) {
        fprintf(mesg, "zip diagnostic: GetVolumeInformation failed\n");
        return(FALSE);
    }

    lastDrive = root[0];
    lastDriveOldFAT = vfnsize <= 12;

    return lastDriveOldFAT;
}
#endif


/* access mode bits and time stamp */

int GetFileMode(char *name)
{
DWORD dwAttr;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  char *ansi_name = (char *)alloca(strlen(name) + 1);

  OemToAnsi(name, ansi_name);
  name = ansi_name;
#endif

  dwAttr = GetFileAttributes(name);
  if ( dwAttr == 0xFFFFFFFF ) {
    zipwarn("reading file attributes failed: ", name);
    /*
    fprintf(mesg, "zip diagnostic: GetFileAttributes failed");
    fflush();
    */
    return(0x20); /* the most likely, though why the error? security? */
  }
  return(
          (dwAttr&FILE_ATTRIBUTE_READONLY  ? A_RONLY   :0)
        | (dwAttr&FILE_ATTRIBUTE_HIDDEN    ? A_HIDDEN  :0)
        | (dwAttr&FILE_ATTRIBUTE_SYSTEM    ? A_SYSTEM  :0)
        | (dwAttr&FILE_ATTRIBUTE_DIRECTORY ? A_DIR     :0)
        | (dwAttr&FILE_ATTRIBUTE_ARCHIVE   ? A_ARCHIVE :0));
}

#ifdef UNICODE_SUPPORT
int GetFileModeW(wchar_t *namew)
{
DWORD dwAttr;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  wchar_t *ansi_namew = (wchar_t *)alloca((wcslen(namew) + 1) * sizeof(wchar_t));

  CharToAnsiW(namew, ansi_namew);
  namew = ansi_namew;
#endif

  dwAttr = GetFileAttributesW(namew);
  if ( dwAttr == 0xFFFFFFFF ) {
    char *name = wchar_to_local_string(namew);
    zipwarn("reading file attributes failed: ", name);
    free(name);
    return(0x20); /* the most likely, though why the error? security? */
  }
  return(
          (dwAttr&FILE_ATTRIBUTE_READONLY  ? A_RONLY   :0)
        | (dwAttr&FILE_ATTRIBUTE_HIDDEN    ? A_HIDDEN  :0)
        | (dwAttr&FILE_ATTRIBUTE_SYSTEM    ? A_SYSTEM  :0)
        | (dwAttr&FILE_ATTRIBUTE_DIRECTORY ? A_DIR     :0)
        | (dwAttr&FILE_ATTRIBUTE_ARCHIVE   ? A_ARCHIVE :0));
}
#endif


int ClearArchiveBitW(wchar_t *namew)
{
DWORD dwAttr;
  dwAttr = GetFileAttributesW(namew);
  if ( dwAttr == 0xFFFFFFFF ) {
    fprintf(mesg, "zip diagnostic: GetFileAttributes failed\n");
    return(0);
  }

  if (!SetFileAttributesW(namew, (DWORD)(dwAttr & ~FILE_ATTRIBUTE_ARCHIVE))) {
    fprintf(mesg, "zip diagnostic: SetFileAttributes failed\n");
    perror("SetFileAttributes");
    return(0);
  }
  return(1);
}

int ClearArchiveBit(char *name)
{
DWORD dwAttr;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  char *ansi_name = (char *)alloca(strlen(name) + 1);

  OemToAnsi(name, ansi_name);
  name = ansi_name;
#endif

  dwAttr = GetFileAttributes(name);
  if ( dwAttr == 0xFFFFFFFF ) {
    fprintf(mesg, "zip diagnostic: GetFileAttributes failed\n");
    return(0);
  }

  if (!SetFileAttributes(name, (DWORD)(dwAttr & ~FILE_ATTRIBUTE_ARCHIVE))) {
    fprintf(mesg, "zip diagnostic: SetFileAttributes failed\n");
    perror("SetFileAttributes");
    return(0);
  }
  return(1);
}


#ifdef NT_TZBUG_WORKAROUND
local int FSusesLocalTime(const char *path)
{
    char  *tmp0;
    char   rootPathName[4];
    char   tmp1[MAX_PATH], tmp2[MAX_PATH];
    DWORD  volSerNo, maxCompLen, fileSysFlags;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_path = (char *)alloca(strlen(path) + 1);

    OemToAnsi(path, ansi_path);
    path = ansi_path;
#endif

    if (isalpha((uch)path[0]) && (path[1] == ':'))
        tmp0 = (char *)path;
    else
    {
        GetFullPathName(path, MAX_PATH, tmp1, &tmp0);
        tmp0 = &tmp1[0];
    }
    strncpy(rootPathName, tmp0, 3);   /* Build the root path name, */
    rootPathName[3] = '\0';           /* e.g. "A:/"                */

    GetVolumeInformation((LPCTSTR)rootPathName, (LPTSTR)tmp1, (DWORD)MAX_PATH,
                         &volSerNo, &maxCompLen, &fileSysFlags,
                         (LPTSTR)tmp2, (DWORD)MAX_PATH);

    /* Volumes in (V)FAT and (OS/2) HPFS format store file timestamps in
     * local time!
     */
    return !strncmp(strupr(tmp2), "FAT", 3) ||
           !strncmp(tmp2, "VFAT", 4) ||
           !strncmp(tmp2, "HPFS", 4);

} /* end function FSusesLocalTime() */

# ifdef UNICODE_SUPPORT
local int FSusesLocalTimeW(const wchar_t *path)
{
    wchar_t  *tmp0;
    wchar_t   rootPathName[4];
    wchar_t   tmp1[MAX_PATH], tmp2[MAX_PATH];
    DWORD  volSerNo, maxCompLen, fileSysFlags;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
    wchar_t *ansi_path = (wchar_t *)alloca((wcslen(path) + 1) * sizeof(wchar_t));

    CharToAnsiW(path, ansi_path);
    path = ansi_path;
#endif

    if (iswalpha(path[0]) && (path[1] == (wchar_t)':'))
        tmp0 = (wchar_t *)path;
    else
    {
        GetFullPathNameW(path, MAX_PATH, tmp1, &tmp0);
        tmp0 = &tmp1[0];
    }
    wcsncpy(rootPathName, tmp0, 3);   /* Build the root path name, */
    rootPathName[3] = (wchar_t)'\0';           /* e.g. "A:/"                */

    GetVolumeInformationW(rootPathName, tmp1, (DWORD)MAX_PATH,
                         &volSerNo, &maxCompLen, &fileSysFlags,
                         tmp2, (DWORD)MAX_PATH);

    /* Volumes in (V)FAT and (OS/2) HPFS format store file timestamps in
     * local time!
     */
    return !wcsncmp(_wcsupr(tmp2), L"FAT", 3) ||
           !wcsncmp(tmp2, L"VFAT", 4) ||
           !wcsncmp(tmp2, L"HPFS", 4);

} /* end function FSusesLocalTimeW() */
# endif

#endif /* NT_TZBUG_WORKAROUND */


#if (defined(USE_EF_UT_TIME) || defined(NT_TZBUG_WORKAROUND))

#if (defined(__GNUC__) || defined(ULONG_LONG_MAX))
   typedef long long            LLONG64;
   typedef unsigned long long   ULLNG64;
#elif (defined(__WATCOMC__) && (__WATCOMC__ >= 1100))
   typedef __int64              LLONG64;
   typedef unsigned __int64     ULLNG64;
#elif (defined(_MSC_VER) && (_MSC_VER >= 1100))
   typedef __int64              LLONG64;
   typedef unsigned __int64     ULLNG64;
#elif (defined(__IBMC__) && (__IBMC__ >= 350))
   typedef __int64              LLONG64;
   typedef unsigned __int64     ULLNG64;
#else
#  define NO_INT64
#endif

#  define UNIX_TIME_ZERO_HI  0x019DB1DEUL
#  define UNIX_TIME_ZERO_LO  0xD53E8000UL
#  define NT_QUANTA_PER_UNIX 10000000L
#  define FTQUANTA_PER_UT_L  (NT_QUANTA_PER_UNIX & 0xFFFF)
#  define FTQUANTA_PER_UT_H  (NT_QUANTA_PER_UNIX >> 16)
#  define UNIX_TIME_UMAX_HI  0x0236485EUL
#  define UNIX_TIME_UMAX_LO  0xD4A5E980UL
#  define UNIX_TIME_SMIN_HI  0x0151669EUL
#  define UNIX_TIME_SMIN_LO  0xD53E8000UL
#  define UNIX_TIME_SMAX_HI  0x01E9FD1EUL
#  define UNIX_TIME_SMAX_LO  0xD4A5E980UL

local int FileTime2utime(FILETIME *pft, time_t *ut)
{
#ifndef NO_INT64
    ULLNG64 NTtime;

    NTtime = ((ULLNG64)pft->dwLowDateTime +
              ((ULLNG64)pft->dwHighDateTime << 32));

    /* underflow and overflow handling */
#ifdef CHECK_UTIME_SIGNED_UNSIGNED
    if ((time_t)0x80000000L < (time_t)0L)
    {
        if (NTtime < ((ULLNG64)UNIX_TIME_SMIN_LO +
                      ((ULLNG64)UNIX_TIME_SMIN_HI << 32))) {
            *ut = (time_t)LONG_MIN;
            return FALSE;
        }
        if (NTtime > ((ULLNG64)UNIX_TIME_SMAX_LO +
                      ((ULLNG64)UNIX_TIME_SMAX_HI << 32))) {
            *ut = (time_t)LONG_MAX;
            return FALSE;
        }
    }
    else
#endif /* CHECK_UTIME_SIGNED_UNSIGNED */
    {
        if (NTtime < ((ULLNG64)UNIX_TIME_ZERO_LO +
                      ((ULLNG64)UNIX_TIME_ZERO_HI << 32))) {
            *ut = (time_t)0;
            return FALSE;
        }
        if (NTtime > ((ULLNG64)UNIX_TIME_UMAX_LO +
                      ((ULLNG64)UNIX_TIME_UMAX_HI << 32))) {
            *ut = (time_t)ULONG_MAX;
            return FALSE;
        }
    }

    NTtime -= ((ULLNG64)UNIX_TIME_ZERO_LO +
               ((ULLNG64)UNIX_TIME_ZERO_HI << 32));
    *ut = (time_t)(NTtime / (unsigned long)NT_QUANTA_PER_UNIX);
    return TRUE;
#else /* NO_INT64 (64-bit integer arithmetics may not be supported) */
    /* nonzero if `y' is a leap year, else zero */
#   define leap(y) (((y)%4 == 0 && (y)%100 != 0) || (y)%400 == 0)
    /* number of leap years from 1970 to `y' (not including `y' itself) */
#   define nleap(y) (((y)-1969)/4 - ((y)-1901)/100 + ((y)-1601)/400)
    /* daycount at the end of month[m-1] */
    static ZCONST ush ydays[] =
      { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

    time_t days;
    SYSTEMTIME w32tm;

    /* underflow and overflow handling */
#ifdef CHECK_UTIME_SIGNED_UNSIGNED
    if ((time_t)0x80000000L < (time_t)0L)
    {
        if ((pft->dwHighDateTime < UNIX_TIME_SMIN_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_SMIN_HI) &&
             (pft->dwLowDateTime < UNIX_TIME_SMIN_LO))) {
            *ut = (time_t)LONG_MIN;
            return FALSE;
        if ((pft->dwHighDateTime > UNIX_TIME_SMAX_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_SMAX_HI) &&
             (pft->dwLowDateTime > UNIX_TIME_SMAX_LO))) {
            *ut = (time_t)LONG_MAX;
            return FALSE;
        }
    }
    else
#endif /* CHECK_UTIME_SIGNED_UNSIGNED */
    {
        if ((pft->dwHighDateTime < UNIX_TIME_ZERO_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_ZERO_HI) &&
             (pft->dwLowDateTime < UNIX_TIME_ZERO_LO))) {
            *ut = (time_t)0;
            return FALSE;
        }
        if ((pft->dwHighDateTime > UNIX_TIME_UMAX_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_UMAX_HI) &&
             (pft->dwLowDateTime > UNIX_TIME_UMAX_LO))) {
            *ut = (time_t)ULONG_MAX;
            return FALSE;
        }
    }

    FileTimeToSystemTime(pft, &w32tm);

    /* set `days' to the number of days into the year */
    days = w32tm.wDay - 1 + ydays[w32tm.wMonth-1] +
           (w32tm.wMonth > 2 && leap (w32tm.wYear));

    /* now set `days' to the number of days since 1 Jan 1970 */
    days += 365 * (time_t)(w32tm.wYear - 1970) +
            (time_t)(nleap(w32tm.wYear));

    *ut = (time_t)(86400L * days + 3600L * (time_t)w32tm.wHour +
                   (time_t)(60 * w32tm.wMinute + w32tm.wSecond));
    return TRUE;
#endif /* ?NO_INT64 */
} /* end function FileTime2utime() */
#endif /* USE_EF_UT_TIME || NT_TZBUG_WORKAROUND */


#if (defined(NT_TZBUG_WORKAROUND) && defined(W32_STAT_BANDAID))

local int VFatFileTime2utime(const FILETIME *pft, time_t *ut)
{
    FILETIME lft;
    SYSTEMTIME w32tm;
    struct tm ltm;

    FileTimeToLocalFileTime(pft, &lft);
    FileTimeToSystemTime(&lft, &w32tm);
    /* underflow and overflow handling */
    /* TODO: The range checks are not accurate, the actual limits may
     *       be off by one daylight-saving-time shift (typically 1 hour),
     *       depending on the current state of "is_dst".
     */
#ifdef CHECK_UTIME_SIGNED_UNSIGNED
    if ((time_t)0x80000000L < (time_t)0L)
    {
        if ((pft->dwHighDateTime < UNIX_TIME_SMIN_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_SMIN_HI) &&
             (pft->dwLowDateTime < UNIX_TIME_SMIN_LO))) {
            *ut = (time_t)LONG_MIN;
            return FALSE;
        if ((pft->dwHighDateTime > UNIX_TIME_SMAX_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_SMAX_HI) &&
             (pft->dwLowDateTime > UNIX_TIME_SMAX_LO))) {
            *ut = (time_t)LONG_MAX;
            return FALSE;
        }
    }
    else
#endif /* CHECK_UTIME_SIGNED_UNSIGNED */
    {
        if ((pft->dwHighDateTime < UNIX_TIME_ZERO_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_ZERO_HI) &&
             (pft->dwLowDateTime < UNIX_TIME_ZERO_LO))) {
            *ut = (time_t)0;
            return FALSE;
        }
        if ((pft->dwHighDateTime > UNIX_TIME_UMAX_HI) ||
            ((pft->dwHighDateTime == UNIX_TIME_UMAX_HI) &&
             (pft->dwLowDateTime > UNIX_TIME_UMAX_LO))) {
            *ut = (time_t)ULONG_MAX;
            return FALSE;
        }
    }
    ltm.tm_year = w32tm.wYear - 1900;
    ltm.tm_mon = w32tm.wMonth - 1;
    ltm.tm_mday = w32tm.wDay;
    ltm.tm_hour = w32tm.wHour;
    ltm.tm_min = w32tm.wMinute;
    ltm.tm_sec = w32tm.wSecond;
    ltm.tm_isdst = -1;  /* let mktime determine if DST is in effect */
    *ut = mktime(&ltm);

    /* a cheap error check: mktime returns "(time_t)-1L" on conversion errors.
     * Normally, we would have to apply a consistency check because "-1"
     * could also be a valid time. But, it is quite unlikely to read back odd
     * time numbers from file systems that store time stamps in DOS format.
     * (The only known exception is creation time on VFAT partitions.)
     */
    return (*ut != (time_t)-1L);

} /* end function VFatFileTime2utime() */
#endif /* NT_TZBUG_WORKAROUND && W32_STAT_BANDAID */


#if 0           /* Currently, this is not used at all */

long GetTheFileTime(char *name, iztimes *z_ut)
{
HANDLE h;
FILETIME Modft, Accft, Creft, lft;
WORD dh, dl;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
  char *ansi_name = (char *)alloca(strlen(name) + 1);

  OemToAnsi(name, ansi_name);
  name = ansi_name;
#endif

  h = CreateFile(name, FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
                 NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if ( h != INVALID_HANDLE_VALUE ) {
    BOOL ftOK = GetFileTime(h, &Creft, &Accft, &Modft);
    CloseHandle(h);
#ifdef USE_EF_UT_TIME
    if (ftOK && (z_ut != NULL)) {
      FileTime2utime(&Modft, &(z_ut->mtime));
      if (Accft.dwLowDateTime != 0 || Accft.dwHighDateTime != 0)
          FileTime2utime(&Accft, &(z_ut->atime));
      else
          z_ut->atime = z_ut->mtime;
      if (Creft.dwLowDateTime != 0 || Creft.dwHighDateTime != 0)
          FileTime2utime(&Creft, &(z_ut->ctime));
      else
          z_ut->ctime = z_ut->mtime;
    }
#endif
    FileTimeToLocalFileTime(&ft, &lft);
    FileTimeToDosDateTime(&lft, &dh, &dl);
    return(dh<<16) | dl;
  }
  else
    return 0L;
}

#endif /* never */


void ChangeNameForFAT(char *name)
{
  char *src, *dst, *next, *ptr, *dot, *start;
  static char invalid[] = ":;,=+\"[]<>| \t";

  if ( isalpha((uch)name[0]) && (name[1] == ':') )
    start = name + 2;
  else
    start = name;

  src = dst = start;
  if ( (*src == '/') || (*src == '\\') )
    src++, dst++;

  while ( *src )
  {
    for ( next = src; *next && (*next != '/') && (*next != '\\'); next++ );

    for ( ptr = src, dot = NULL; ptr < next; ptr++ )
      if ( *ptr == '.' )
      {
        dot = ptr; /* remember last dot */
        *ptr = '_';
      }

    if ( dot == NULL )
      for ( ptr = src; ptr < next; ptr++ )
        if ( *ptr == '_' )
          dot = ptr; /* remember last _ as if it were a dot */

    if ( dot && (dot > src) &&
         ((next - dot <= 4) ||
          ((next - src > 8) && (dot - src > 3))) )
    {
      if ( dot )
        *dot = '.';

      for ( ptr = src; (ptr < dot) && ((ptr - src) < 8); ptr++ )
        *dst++ = *ptr;

      for ( ptr = dot; (ptr < next) && ((ptr - dot) < 4); ptr++ )
        *dst++ = *ptr;
    }
    else
    {
      if ( dot && (next - src == 1) )
        *dot = '.';           /* special case: "." as a path component */

      for ( ptr = src; (ptr < next) && ((ptr - src) < 8); ptr++ )
        *dst++ = *ptr;
    }

    *dst++ = *next; /* either '/' or 0 */

    if ( *next )
    {
      src = next + 1;

      if ( *src == 0 ) /* handle trailing '/' on dirs ! */
        *dst = 0;
    }
    else
      break;
  }

  for ( src = start; *src != 0; ++src )
    if ( (strchr(invalid, *src) != NULL) || (*src == ' ') )
      *src = '_';
}

char *GetLongPathEA(char *name)
{
    return(NULL); /* volunteers ? */
}

#ifdef UNICODE_SUPPORT
wchar_t *GetLongPathEAW(wchar_t *name)
{
    return(NULL); /* volunteers ? */
}
#endif


int IsFileNameValid(x)
char *x;
{
    WIN32_FIND_DATA fd;
    HANDLE h;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_name = (char *)alloca(strlen(x) + 1);

    OemToAnsi(x, ansi_name);
    x = ansi_name;
#endif

    if ((h = FindFirstFile(x, &fd)) == INVALID_HANDLE_VALUE)
        return FALSE;
    FindClose(h);
    return TRUE;
}

char *getVolumeLabel(drive, vtime, vmode, vutim)
  int drive;    /* drive name: 'A' .. 'Z' or '\0' for current drive */
  ulg *vtime;   /* volume label creation time (DOS format) */
  ulg *vmode;   /* volume label file mode */
  time_t *vutim;/* volume label creationtime (UNIX format) */

/* If a volume label exists for the given drive, return its name and
   pretend to set its time and mode. The returned name is static data. */
{
  char rootpath[4];
  static char vol[14];
  DWORD fnlen, flags;

  *vmode = A_ARCHIVE | A_LABEL;           /* this is what msdos returns */
  *vtime = dostime(1980, 1, 1, 0, 0, 0);  /* no true date info available */
  *vutim = dos2unixtime(*vtime);
  strcpy(rootpath, "x:\\");
  rootpath[0] = (char)drive;
  if (GetVolumeInformation(drive ? rootpath : NULL, vol, 13, NULL,
                           &fnlen, &flags, NULL, 0))
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
    return (AnsiToOem(vol, vol), vol);
#else
    return vol;
#endif
  else
    return NULL;
}

#endif /* !UTIL */



int ZipIsWinNT(void)    /* returns TRUE if real NT, FALSE if Win95 or Win32s */
{
    static DWORD g_PlatformId = 0xFFFFFFFF; /* saved platform indicator */

    if (g_PlatformId == 0xFFFFFFFF) {
        /* note: GetVersionEx() doesn't exist on WinNT 3.1 */
        if (GetVersion() < 0x80000000)
            g_PlatformId = TRUE;
        else
            g_PlatformId = FALSE;
    }
    return (int)g_PlatformId;
}


#ifndef UTIL
#ifdef __WATCOMC__
#  include <io.h>
#  define _get_osfhandle _os_handle
/* gaah -- Watcom's docs claim that _get_osfhandle exists, but it doesn't.  */
#endif

#ifdef HAVE_FSEEKABLE
/*
 * return TRUE if file is seekable
 */
int fseekable(fp)
FILE *fp;
{
    return GetFileType((HANDLE)_get_osfhandle(fileno(fp))) == FILE_TYPE_DISK;
}
#endif /* HAVE_FSEEKABLE */
#endif /* !UTIL */


#if 0 /* seems to be never used; try it out... */
char *StringLower(char *szArg)
{
  char *szPtr;
/*  unsigned char *szPtr; */
  for ( szPtr = szArg; *szPtr; szPtr++ )
    *szPtr = lower[*szPtr];
  return szArg;
}
#endif /* never */



#ifdef W32_STAT_BANDAID

/* All currently known variants of WIN32 operating systems (Windows 95/98,
 * WinNT 3.x, 4.0, 5.0) have a nasty bug in the OS kernel concerning
 * conversions between UTC and local time: In the time conversion functions
 * of the Win32 API, the timezone offset (including seasonal daylight saving
 * shift) between UTC and local time evaluation is erratically based on the
 * current system time. The correct evaluation must determine the offset
 * value as it {was/is/will be} for the actual time to be converted.
 *
 * The C runtime lib's stat() returns utc time-stamps so that
 * localtime(timestamp) corresponds to the (potentially false) local
 * time shown by the OS' system programs (Explorer, command shell dir, etc.)
 *
 * For the NTFS file system (and other filesystems that store time-stamps
 * as UTC values), this results in st_mtime (, st_{c|a}time) fields which
 * are not stable but vary according to the seasonal change of "daylight
 * saving time in effect / not in effect".
 *
 * To achieve timestamp consistency of UTC (UT extra field) values in
 * Zip archives, the Info-ZIP programs require work-around code for
 * proper time handling in stat() (and other time handling routines).
 */
/* stat() functions under Windows95 tend to fail for root directories.   *
 * Watcom and Borland, at least, are affected by this bug.  Watcom made  *
 * a partial fix for 11.0 but still missed some cases.  This substitute  *
 * detects the case and fills in reasonable values.  Otherwise we get    *
 * effects like failure to extract to a root dir because it's not found. */

#ifdef LARGE_FILE_SUPPORT         /* E. Gordon 9/12/03 */
 int zstat_zipwin32(const char *path, z_stat *buf)
#else
 int zstat_zipwin32(const char *path, struct stat *buf)
#endif
{
# ifdef LARGE_FILE_SUPPORT         /* E. Gordon 9/12/03 */
    if (!zstat(path, buf))
# else
    if (!stat(path, buf))
# endif
    {
#ifdef NT_TZBUG_WORKAROUND
        /* stat was successful, now redo the time-stamp fetches */
        int fs_uses_loctime = FSusesLocalTime(path);
        HANDLE h;
        FILETIME Modft, Accft, Creft;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
        char *ansi_path = (char *)alloca(strlen(path) + 1);

        OemToAnsi(path, ansi_path);
#       define Ansi_Path  ansi_path
#else
#       define Ansi_Path  path
#endif

        Trace((stdout, "stat(%s) finds modtime %08lx\n", path, buf->st_mtime));
        h = CreateFile(Ansi_Path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
                       NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            BOOL ftOK = GetFileTime(h, &Creft, &Accft, &Modft);
            CloseHandle(h);

            if (ftOK) {
                if (!fs_uses_loctime) {
                    /*  On a filesystem that stores UTC timestamps, we refill
                     *  the time fields of the struct stat buffer by directly
                     *  using the UTC values as returned by the Win32
                     *  GetFileTime() API call.
                     */
                    FileTime2utime(&Modft, &(buf->st_mtime));
                    if (Accft.dwLowDateTime != 0 || Accft.dwHighDateTime != 0)
                        FileTime2utime(&Accft, &(buf->st_atime));
                    else
                        buf->st_atime = buf->st_mtime;
                    if (Creft.dwLowDateTime != 0 || Creft.dwHighDateTime != 0)
                        FileTime2utime(&Creft, &(buf->st_ctime));
                    else
                        buf->st_ctime = buf->st_mtime;
                    Tracev((stdout,"NTFS, recalculated modtime %08lx\n",
                            buf->st_mtime));
                } else {
                    /*  On VFAT and FAT-like filesystems, the FILETIME values
                     *  are converted back to the stable local time before
                     *  converting them to UTC unix time-stamps.
                     */
                    VFatFileTime2utime(&Modft, &(buf->st_mtime));
                    if (Accft.dwLowDateTime != 0 || Accft.dwHighDateTime != 0)
                        VFatFileTime2utime(&Accft, &(buf->st_atime));
                    else
                        buf->st_atime = buf->st_mtime;
                    if (Creft.dwLowDateTime != 0 || Creft.dwHighDateTime != 0)
                        VFatFileTime2utime(&Creft, &(buf->st_ctime));
                    else
                        buf->st_ctime = buf->st_mtime;
                    Tracev((stdout, "VFAT, recalculated modtime %08lx\n",
                            buf->st_mtime));
                }
            }
        }
#       undef Ansi_Path
#endif /* NT_TZBUG_WORKAROUND */
        return 0;
    }
#ifdef W32_STATROOT_FIX
    else
    {
        DWORD flags;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
        char *ansi_path = (char *)alloca(strlen(path) + 1);

        OemToAnsi(path, ansi_path);
#       define Ansi_Path  ansi_path
#else
#       define Ansi_Path  path
#endif

        flags = GetFileAttributes(Ansi_Path);
        if (flags != 0xFFFFFFFF && flags & FILE_ATTRIBUTE_DIRECTORY) {
            Trace((stderr, "\nstat(\"%s\",...) failed on existing directory\n",
                   path));
#ifdef LARGE_FILE_SUPPORT         /* E. Gordon 9/12/03 */
            memset(buf, 0, sizeof(z_stat));
#else
            memset(buf, 0, sizeof(struct stat));
#endif
            buf->st_atime = buf->st_ctime = buf->st_mtime =
              dos2unixtime(DOSTIME_MINIMUM);
            /* !!!   you MUST NOT add a cast to the type of "st_mode" here;
             * !!!   different compilers do not agree on the "st_mode" size!
             * !!!   (And, some compiler may not declare the "mode_t" type
             * !!!   identifier, so you cannot use it, either.)
             */
            buf->st_mode = S_IFDIR | S_IREAD |
                           ((flags & FILE_ATTRIBUTE_READONLY) ? 0 : S_IWRITE);
            return 0;
        } /* assumes: stat() won't fail on non-dirs without good reason */
#       undef Ansi_Path
    }
#endif /* W32_STATROOT_FIX */
    return -1;
}


# ifdef UNICODE_SUPPORT

int zstat_zipwin32w(const wchar_t *pathw, zw_stat *buf)
{
    if (!zwstat(pathw, buf))
    {
#ifdef NT_TZBUG_WORKAROUND
        /* stat was successful, now redo the time-stamp fetches */
        int fs_uses_loctime = FSusesLocalTimeW(pathw);
        HANDLE h;
        FILETIME Modft, Accft, Creft;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
        char *ansi_path = (char *)alloca(strlen(pathw) + 1);

        OemToAnsi(path, ansi_path);
#       define Ansi_Path  ansi_path
#else
#       define Ansi_Path  pathw
#endif

        Trace((stdout, "stat(%s) finds modtime %08lx\n", pathw, buf->st_mtime));
        h = CreateFileW(Ansi_Path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
                       NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            BOOL ftOK = GetFileTime(h, &Creft, &Accft, &Modft);
            CloseHandle(h);

            if (ftOK) {
                if (!fs_uses_loctime) {
                    /*  On a filesystem that stores UTC timestamps, we refill
                     *  the time fields of the struct stat buffer by directly
                     *  using the UTC values as returned by the Win32
                     *  GetFileTime() API call.
                     */
                    FileTime2utime(&Modft, &(buf->st_mtime));
                    if (Accft.dwLowDateTime != 0 || Accft.dwHighDateTime != 0)
                        FileTime2utime(&Accft, &(buf->st_atime));
                    else
                        buf->st_atime = buf->st_mtime;
                    if (Creft.dwLowDateTime != 0 || Creft.dwHighDateTime != 0)
                        FileTime2utime(&Creft, &(buf->st_ctime));
                    else
                        buf->st_ctime = buf->st_mtime;
                    Tracev((stdout,"NTFS, recalculated modtime %08lx\n",
                            buf->st_mtime));
                } else {
                    /*  On VFAT and FAT-like filesystems, the FILETIME values
                     *  are converted back to the stable local time before
                     *  converting them to UTC unix time-stamps.
                     */
                    VFatFileTime2utime(&Modft, &(buf->st_mtime));
                    if (Accft.dwLowDateTime != 0 || Accft.dwHighDateTime != 0)
                        VFatFileTime2utime(&Accft, &(buf->st_atime));
                    else
                        buf->st_atime = buf->st_mtime;
                    if (Creft.dwLowDateTime != 0 || Creft.dwHighDateTime != 0)
                        VFatFileTime2utime(&Creft, &(buf->st_ctime));
                    else
                        buf->st_ctime = buf->st_mtime;
                    Tracev((stdout, "VFAT, recalculated modtime %08lx\n",
                            buf->st_mtime));
                }
            }
        }
#       undef Ansi_Path
#endif /* NT_TZBUG_WORKAROUND */
        return 0;
    }
#ifdef W32_STATROOT_FIX
    else
    {
        DWORD flags;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
        char *ansi_path = (char *)alloca(strlen(pathw) + 1);

        OemToAnsi(path, ansi_path);
#       define Ansi_Path  ansi_path
#else
#       define Ansi_Path  pathw
#endif

        flags = GetFileAttributesW(Ansi_Path);
        if (flags != 0xFFFFFFFF && flags & FILE_ATTRIBUTE_DIRECTORY) {
            Trace((stderr, "\nstat(\"%s\",...) failed on existing directory\n",
                   pathw));
#ifdef LARGE_FILE_SUPPORT         /* E. Gordon 9/12/03 */
            memset(buf, 0, sizeof(z_stat));
#else
            memset(buf, 0, sizeof(struct stat));
#endif
            buf->st_atime = buf->st_ctime = buf->st_mtime =
              dos2unixtime(DOSTIME_MINIMUM);
            /* !!!   you MUST NOT add a cast to the type of "st_mode" here;
             * !!!   different compilers do not agree on the "st_mode" size!
             * !!!   (And, some compiler may not declare the "mode_t" type
             * !!!   identifier, so you cannot use it, either.)
             */
            buf->st_mode = S_IFDIR | S_IREAD |
                           ((flags & FILE_ATTRIBUTE_READONLY) ? 0 : S_IWRITE);
            return 0;
        } /* assumes: stat() won't fail on non-dirs without good reason */
#       undef Ansi_Path
    }
#endif /* W32_STATROOT_FIX */
    return -1;
}

# endif


#endif /* W32_STAT_BANDAID */



#ifdef W32_USE_IZ_TIMEZONE
#include "timezone.h"
#define SECSPERMIN      60
#define MINSPERHOUR     60
#define SECSPERHOUR     (SECSPERMIN * MINSPERHOUR)
static void conv_to_rule(LPSYSTEMTIME lpw32tm, struct rule * ZCONST ptrule);

static void conv_to_rule(LPSYSTEMTIME lpw32tm, struct rule * ZCONST ptrule)
{
    if (lpw32tm->wYear != 0) {
        ptrule->r_type = JULIAN_DAY;
        ptrule->r_day = ydays[lpw32tm->wMonth - 1] + lpw32tm->wDay;
    } else {
        ptrule->r_type = MONTH_NTH_DAY_OF_WEEK;
        ptrule->r_mon = lpw32tm->wMonth;
        ptrule->r_day = lpw32tm->wDayOfWeek;
        ptrule->r_week = lpw32tm->wDay;
    }
    ptrule->r_time = (long)lpw32tm->wHour * SECSPERHOUR +
                     (long)(lpw32tm->wMinute * SECSPERMIN) +
                     (long)lpw32tm->wSecond;
}

int GetPlatformLocalTimezone(register struct state * ZCONST sp,
        void (*fill_tzstate_from_rules)(struct state * ZCONST sp_res,
                                        ZCONST struct rule * ZCONST start,
                                        ZCONST struct rule * ZCONST end))
{
    TIME_ZONE_INFORMATION tzinfo;
    DWORD res;

    /* read current timezone settings from registry if TZ envvar missing */
    res = GetTimeZoneInformation(&tzinfo);
    if (res != TIME_ZONE_ID_INVALID)
    {
        struct rule startrule, stoprule;

        conv_to_rule(&(tzinfo.StandardDate), &stoprule);
        conv_to_rule(&(tzinfo.DaylightDate), &startrule);
        sp->timecnt = 0;
        sp->ttis[0].tt_abbrind = 0;
        if ((sp->charcnt =
             WideCharToMultiByte(CP_ACP, 0, tzinfo.StandardName, -1,
                                 sp->chars, sizeof(sp->chars), NULL, NULL))
            == 0)
            sp->chars[sp->charcnt++] = '\0';
        sp->ttis[1].tt_abbrind = sp->charcnt;
        sp->charcnt +=
            WideCharToMultiByte(CP_ACP, 0, tzinfo.DaylightName, -1,
                                sp->chars + sp->charcnt,
                                sizeof(sp->chars) - sp->charcnt, NULL, NULL);
        if ((sp->charcnt - sp->ttis[1].tt_abbrind) == 0)
            sp->chars[sp->charcnt++] = '\0';
        sp->ttis[0].tt_gmtoff = - (tzinfo.Bias + tzinfo.StandardBias)
                                * MINSPERHOUR;
        sp->ttis[1].tt_gmtoff = - (tzinfo.Bias + tzinfo.DaylightBias)
                                * MINSPERHOUR;
        sp->ttis[0].tt_isdst = 0;
        sp->ttis[1].tt_isdst = 1;
        sp->typecnt = (startrule.r_mon == 0 && stoprule.r_mon == 0) ? 1 : 2;

        if (sp->typecnt > 1)
            (*fill_tzstate_from_rules)(sp, &startrule, &stoprule);
        return TRUE;
    }
    return FALSE;
}
#endif /* W32_USE_IZ_TIMEZONE */



#ifndef WINDLL
/* This replacement getch() function was originally created for Watcom C
 * and then additionally used with CYGWIN. Since UnZip 5.4, all other Win32
 * ports apply this replacement rather that their supplied getch() (or
 * alike) function.  There are problems with unabsorbed LF characters left
 * over in the keyboard buffer under Win95 (and 98) when ENTER was pressed.
 * (Under Win95, ENTER returns two(!!) characters: CR-LF.)  This problem
 * does not appear when run on a WinNT console prompt!
 */

/* Watcom 10.6's getch() does not handle Alt+<digit><digit><digit>. */
/* Note that if PASSWD_FROM_STDIN is defined, the file containing   */
/* the password must have a carriage return after the word, not a   */
/* Unix-style newline (linefeed only).  This discards linefeeds.    */

int getch_win32(void)
{
  HANDLE stin;
  DWORD rc;
  unsigned char buf[2];
  int ret = -1;
  DWORD odemode = ~(DWORD)0;

#  ifdef PASSWD_FROM_STDIN
  stin = GetStdHandle(STD_INPUT_HANDLE);
#  else
  stin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (stin == INVALID_HANDLE_VALUE)
    return -1;
#  endif
  if (GetConsoleMode(stin, &odemode))
    SetConsoleMode(stin, ENABLE_PROCESSED_INPUT);  /* raw except ^C noticed */
  if (ReadFile(stin, &buf, 1, &rc, NULL) && rc == 1)
    ret = buf[0];
  /* when the user hits return we get CR LF.  We discard the LF, not the CR,
   * because when we call this for the first time after a previous input
   * such as the one for "replace foo? [y]es, ..." the LF may still be in
   * the input stream before whatever the user types at our prompt. */
  if (ret == '\n')
    if (ReadFile(stin, &buf, 1, &rc, NULL) && rc == 1)
      ret = buf[0];
  if (odemode != ~(DWORD)0)
    SetConsoleMode(stin, odemode);
#  ifndef PASSWD_FROM_STDIN
  CloseHandle(stin);
#  endif
  return ret;
}



/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s.\n\n";
#if (defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DJGPP__))
    char buf[80];
#if (defined(_MSC_VER) && (_MSC_VER > 900))
    char buf2[80];
#endif
#endif

/* Define the compiler name and version strings */
#if defined(_MSC_VER)  /* MSC == MSVC++, including the SDK compiler */
    sprintf(buf, "Microsoft C %d.%02d ", _MSC_VER/100, _MSC_VER%100);
#  define COMPILER_NAME1        buf
#  if (_MSC_VER == 800)
#    define COMPILER_NAME2      "(Visual C++ v1.1)"
#  elif (_MSC_VER == 850)
#    define COMPILER_NAME2      "(Windows NT v3.5 SDK)"
#  elif (_MSC_VER == 900)
#    define COMPILER_NAME2      "(Visual C++ v2.x)"
#  elif (_MSC_VER > 900)
    sprintf(buf2, "(Visual C++ v%d.%d)", _MSC_VER/100 - 6, _MSC_VER%100/10);
#    define COMPILER_NAME2      buf2
#  else
#    define COMPILER_NAME2      "(bad version)"
#  endif
#elif defined(__WATCOMC__)
#  if (__WATCOMC__ % 10 > 0)
/* We do this silly test because __WATCOMC__ gives two digits for the  */
/* minor version, but Watcom packaging prefers to show only one digit. */
    sprintf(buf, "Watcom C/C++ %d.%02d", __WATCOMC__ / 100,
            __WATCOMC__ % 100);
#  else
    sprintf(buf, "Watcom C/C++ %d.%d", __WATCOMC__ / 100,
            (__WATCOMC__ % 100) / 10);
#  endif /* __WATCOMC__ % 10 > 0 */
#  define COMPILER_NAME1        buf
#  define COMPILER_NAME2        ""
#elif defined(__TURBOC__)
#  ifdef __BORLANDC__
#    define COMPILER_NAME1      "Borland C++"
#    if (__BORLANDC__ == 0x0452)   /* __BCPLUSPLUS__ = 0x0320 */
#      define COMPILER_NAME2    " 4.0 or 4.02"
#    elif (__BORLANDC__ == 0x0460)   /* __BCPLUSPLUS__ = 0x0340 */
#      define COMPILER_NAME2    " 4.5"
#    elif (__BORLANDC__ == 0x0500)   /* __TURBOC__ = 0x0500 */
#      define COMPILER_NAME2    " 5.0"
#    elif (__BORLANDC__ == 0x0520)   /* __TURBOC__ = 0x0520 */
#      define COMPILER_NAME2    " 5.2 (C++ Builder 1.0)"
#    elif (__BORLANDC__ == 0x0530)   /* __BCPLUSPLUS__ = 0x0530 */
#      define COMPILER_NAME2    " 5.3 (C++ Builder 3.0)"
#    elif (__BORLANDC__ == 0x0540)   /* __BCPLUSPLUS__ = 0x0540 */
#      define COMPILER_NAME2    " 5.4 (C++ Builder 4.0)"
#    elif (__BORLANDC__ == 0x0550)   /* __BCPLUSPLUS__ = 0x0550 */
#      define COMPILER_NAME2    " 5.5 (C++ Builder 5.0)"
#    elif (__BORLANDC__ == 0x0551)   /* __BCPLUSPLUS__ = 0x0551 */
#      define COMPILER_NAME2    " 5.5.1 (C++ Builder 5.0.1)"
#    elif (__BORLANDC__ == 0x0560)   /* __BCPLUSPLUS__ = 0x0560 */
#      define COMPILER_NAME2    " 5.6 (C++ Builder 6)"
#    else
#      define COMPILER_NAME2    " later than 5.6"
#    endif
#  else /* !__BORLANDC__ */
#    define COMPILER_NAME1      "Turbo C"
#    if (__TURBOC__ >= 0x0400)     /* Kevin:  3.0 -> 0x0401 */
#      define COMPILER_NAME2    "++ 3.0 or later"
#    elif (__TURBOC__ == 0x0295)     /* [661] vfy'd by Kevin */
#      define COMPILER_NAME2    "++ 1.0"
#    endif
#  endif /* __BORLANDC__ */
#elif defined(__GNUC__)
#  ifdef __RSXNT__
#    if (defined(__DJGPP__) && !defined(__EMX__))
    sprintf(buf, "rsxnt(djgpp v%d.%02d) / gcc ",
            __DJGPP__, __DJGPP_MINOR__);
#      define COMPILER_NAME1    buf
#    elif defined(__DJGPP__)
   sprintf(buf, "rsxnt(emx+djgpp v%d.%02d) / gcc ",
            __DJGPP__, __DJGPP_MINOR__);
#      define COMPILER_NAME1    buf
#    elif (defined(__GO32__) && !defined(__EMX__))
#      define COMPILER_NAME1    "rsxnt(djgpp v1.x) / gcc "
#    elif defined(__GO32__)
#      define COMPILER_NAME1    "rsxnt(emx + djgpp v1.x) / gcc "
#    elif defined(__EMX__)
#      define COMPILER_NAME1    "rsxnt(emx)+gcc "
#    else
#      define COMPILER_NAME1    "rsxnt(unknown) / gcc "
#    endif
#  elif defined(__CYGWIN__)
#      define COMPILER_NAME1    "Cygnus win32 / gcc "
#  elif defined(__MINGW32__)
#      define COMPILER_NAME1    "mingw32 / gcc "
#  else
#      define COMPILER_NAME1    "gcc "
#  endif
#  define COMPILER_NAME2        __VERSION__
#elif defined(__LCC__)
#  define COMPILER_NAME1        "LCC-Win32"
#  define COMPILER_NAME2        ""
#else
#  define COMPILER_NAME1        "unknown compiler (SDK?)"
#  define COMPILER_NAME2        ""
#endif

/* Define the compile date string */
#ifdef __DATE__
#  define COMPILE_DATE " on " __DATE__
#else
#  define COMPILE_DATE ""
#endif

    printf(CompiledWith, COMPILER_NAME1, COMPILER_NAME2,
           "\nWindows 9x / Windows NT", " (32-bit)", COMPILE_DATE);

    return;

} /* end function version_local() */
#endif /* !WINDLL */


/* --------------------------------------------------- */
/* Large File Support
 *
 * Moved to Win32i64.c to avoid conflicts in same name functions
 * in WiZ using UnZip and Zip libraries.
 * 9/25/2003
 */


/* --------------------------------------------------- */
/* Unicode Support for Win32
 *
 */

#ifdef UNICODE_SUPPORT
# if 0

  /* get the wide command line and convert to argvw */
  /* windows ignores argv and gets argvw separately */
  zchar **get_wide_argv(argv)
    char **argv;
  {
    int i;
    int argc;
    int size;
    zchar **argvw = NULL;
    zchar *commandline = NULL;
    zchar **a = NULL;

    commandline = GetCommandLineW();
    a = CommandLineToArgvW(commandline, &argc);

    if (a == NULL) {
      /* failed */
      ZIPERR(ZE_COMPERR, "get_wide_argv");
    }

    /* copy args so can use free_args() */
    if ((argvw = (zchar **)malloc((argc + 1) * sizeof(zchar *))) == NULL) {
      ZIPERR(ZE_MEM, "get_wide_argv");
    }
    for (i = 0; i < argc; i++) {
      size = zstrlen(a[i]) + 1;
      if ((argvw[i] = (zchar *)malloc(size * sizeof(zchar))) == NULL) {
        ZIPERR(ZE_MEM, "get_wide_argv");
      }
      if ((argvw[i] = copy_zstring(a[i])) == NULL) {
        ZIPERR(ZE_MEM, "get_wide_argv");
      }
    }
    argvw[argc] = L'\0';

    /* free original argvw */
    LocalFree(a);

    return argvw;
  }
# endif


/* convert wide character string to multi-byte character string */
/* win32 version */
char *wide_to_local_string(wide_string)
  zwchar *wide_string;
{
  int i;
  wchar_t wc;
  int bytes_char;
  int default_used;
  int wsize = 0;
  int max_bytes = 9;
  char buf[9];
  char *buffer = NULL;
  char *local_string = NULL;

  if (wide_string == NULL)
    return NULL;

  for (wsize = 0; wide_string[wsize]; wsize++) ;

  if (max_bytes < MB_CUR_MAX)
    max_bytes = MB_CUR_MAX;

  if ((buffer = (char *)malloc(wsize * max_bytes + 1)) == NULL) {
    ZIPERR(ZE_MEM, "wide_to_local_string");
  }

  /* convert it */
  buffer[0] = '\0';
  for (i = 0; i < wsize; i++) {
    if (sizeof(wchar_t) < 4 && wide_string[i] > 0xFFFF) {
      /* wchar_t probably 2 bytes */
      /* could do surrogates if state_dependent and wctomb can do */
      wc = zwchar_to_wchar_t_default_char;
    } else {
      wc = (wchar_t)wide_string[i];
    }
    /* Unter some vendor's C-RTL, the Wide-to-MultiByte conversion functions
     * (like wctomb() et. al.) do not use the same codepage as the other
     * string arguments I/O functions (fopen, mkdir, rmdir etc.).
     * Therefore, we have to fall back to the underlying Win32-API call to
     * achieve a consistent behaviour for all supported compiler environments.
     * Failing RTLs are for example:
     *   Borland (locale uses OEM-CP as default, but I/O functions expect ANSI
     *            names)
     *   Watcom  (only "C" locale, wctomb() always uses OEM CP)
     * (in other words: all supported environments except the Microsoft RTLs)
     */
    bytes_char = WideCharToMultiByte(
                          CP_ACP, WC_COMPOSITECHECK,
                          &wc, 1,
                          (LPSTR)buf, sizeof(buf),
                          NULL, &default_used);
    if (default_used)
      bytes_char = -1;
    if (unicode_escape_all) {
      if (bytes_char == 1 && (uch)buf[0] <= 0x7f) {
        /* ASCII */
        strncat(buffer, buf, 1);
      } else {
        /* use escape for wide character */
        char *e = wide_char_to_escape_string(wide_string[i]);
        strcat(buffer, e);
        free(e);
      }
    } else if (bytes_char > 0) {
      /* multi-byte char */
      strncat(buffer, buf, bytes_char);
    } else {
      /* no MB for this wide */
      if (use_wide_to_mb_default) {
        /* default character */
        strcat(buffer, wide_to_mb_default_string);
      } else {
        /* use escape for wide character */
        char *e = wide_char_to_escape_string(wide_string[i]);
        strcat(buffer, e);
        free(e);
      }
    }
  }
  if ((local_string = (char *)realloc(buffer, strlen(buffer) + 1)) == NULL) {
    free(buffer);
    ZIPERR(ZE_MEM, "wide_to_local_string");
  }

  return local_string;
}

/* convert multi-byte character string to wide character string */
/* win32 version */
zwchar *local_to_wide_string(local_string)
  char *local_string;
{
  int wsize;
  wchar_t *wc_string;
  zwchar *wide_string;

  /* for now try to convert as string - fails if a bad char in string */
  wsize = MultiByteToWideChar(CP_ACP, 0,
                        local_string, -1, NULL, 0);
  if (wsize == (size_t)-1) {
    /* could not convert */
    return NULL;
  }

  /* convert it */
  if ((wc_string = (wchar_t *)malloc((wsize + 1) * sizeof(wchar_t))) == NULL) {
    ZIPERR(ZE_MEM, "local_to_wide_string");
  }
  wsize = MultiByteToWideChar(CP_ACP, 0,
           local_string, -1,
           wc_string, wsize + 1);
  wc_string[wsize] = (wchar_t) 0;

  /* in case wchar_t is not zwchar */
  if ((wide_string = (zwchar *)malloc((wsize + 1) * sizeof(zwchar))) == NULL) {
    free(wc_string);
    ZIPERR(ZE_MEM, "local_to_wide_string");
  }
  for (wsize = 0; (wide_string[wsize] = (zwchar)wc_string[wsize]); wsize++) ;
  wide_string[wsize] = (zwchar)0;
  free(wc_string);

  return wide_string;
}
#endif /* UNICODE_SUPPORT */


/*
# if defined(UNICODE_SUPPORT) || defined(WIN32_OEM)
*/
/* convert oem to ansi character string */
char *oem_to_local_string(local_string, oem_string)
  char *local_string;
  char *oem_string;
{
  /* convert OEM to ANSI character set */
  OemToChar(oem_string, local_string);

  return local_string;
}
/*
# endif
*/


/*
#if defined(UNICODE_SUPPORT) || defined(WIN32_OEM)
*/
/* convert local to oem character string */
char *local_to_oem_string(oem_string, local_string)
  char *oem_string;
  char *local_string;
{
  /* convert to OEM display character set */
  CharToOem(local_string, oem_string);
  return oem_string;
}
/*
#endif
*/

