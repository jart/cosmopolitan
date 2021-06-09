/*
  win32/win32zip.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef UTIL    /* this file contains nothing used by UTIL */

#include "../zip.h"

#include <ctype.h>
#if !defined(__EMX__) && !defined(__CYGWIN__)
#include <direct.h>     /* for rmdir() */
#endif
#include <time.h>

#ifndef __BORLANDC__
#include <sys/utime.h>
#else
#include <utime.h>
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for findfirst/findnext stuff */
#ifdef __RSXNT__
#  include "../win32/rsxntwin.h"
#endif

#include <io.h>

#define PAD           0
#define PATH_END      '/'
#define HIDD_SYS_BITS (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)


#ifdef UNICODE_SUPPORT
typedef struct zdirscanw {
  HANDLE d_hFindFile;
  int    d_first;
  WIN32_FIND_DATAW d_fdw;
} zDIRSCANW;
#endif

typedef struct zdirscan {
  HANDLE d_hFindFile;
  int    d_first;
  WIN32_FIND_DATA d_fd;
} zDIRSCAN;

#define INVALID_WIN32_FILE_ATTRIBS ~0
#ifdef UNICODE_SUPPORT
#define GetDirAttribsW(d)   ((d)->d_fdw.dwFileAttributes)
#endif
#define GetDirAttribs(d)   ((d)->d_fd.dwFileAttributes)

#include "../win32/win32zip.h"
#include "../win32/nt.h"

/* Local functions */
local zDIRSCAN        * OpenDirScan      OF((ZCONST char *n));
local struct zdirscan * GetNextDirEntry  OF((zDIRSCAN *d));
local void              CloseDirScan     OF((zDIRSCAN *d));

#ifdef UNICODE_SUPPORT
local zDIRSCANW        * OpenDirScanW     OF((ZCONST wchar_t *wn));
local struct zdirscanw * GetNextDirEntryW OF((zDIRSCANW *dw));
local void               CloseDirScanW    OF((zDIRSCANW *dw));
#endif

local char           *readd        OF((zDIRSCAN *));
#ifdef UNICODE_SUPPORT
local wchar_t        *readdw       OF((zDIRSCANW *));
#endif

local int             wild_recurse OF((char *, char *));
#ifdef UNICODE_SUPPORT
local int             wild_recursew OF((wchar_t *, wchar_t *));
#endif

#ifdef NTSD_EAS
   local void GetSD OF((char *path, char **bufptr, ush *size,
                        char **cbufptr, ush *csize));
#endif
#ifdef USE_EF_UT_TIME
   local int GetExtraTime OF((struct zlist far *z, iztimes *z_utim));
#endif
local int procname_win32 OF((char *n, int caseflag, DWORD attribs));
#ifdef UNICODE_SUPPORT
local int procname_win32w OF((wchar_t *n, int caseflag, DWORD attribs));
#endif

/* Module level variables */
extern char *label /* = NULL */ ;       /* defined in fileio.c */
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Module level constants */
local ZCONST char wild_match_all[] = "*.*";


#ifdef UNICODE_SUPPORT

local zDIRSCANW *OpenDirScanW(nw)
ZCONST wchar_t *nw;          /* directory to open */
/* Start searching for files in the MSDOS directory n */
{
  zDIRSCANW *dw;         /* malloc'd return value */
  wchar_t *pw;              /* malloc'd temporary string */
  wchar_t *qw;
  size_t i;

  if ((dw = (zDIRSCANW *)malloc(sizeof(zDIRSCANW))) == NULL) {
    return NULL;
  }

  if ((pw = (wchar_t *)malloc(wcslen(nw) * sizeof(wchar_t) +
      (2 + sizeof(wild_match_all)) * sizeof(wchar_t))) == NULL) {
    if (dw != NULL) free((zvoid *)dw);
    return NULL;
  }
  wcscpy(pw, nw);

  qw = pw + wcslen(pw);
  if ((qw - pw) > 0 && wcschr(pw, (wchar_t)':') == (qw - 1))
      *qw++ = (wchar_t)'.';
  if ((qw - pw) > 0 && wcschr(pw, (wchar_t)'/') != (qw - 1))
    *qw++ = (wchar_t)'/';

  for (i = 0; i < strlen(wild_match_all); i++) {
    qw[i] = (wchar_t)wild_match_all[i];
  }
  qw[i] = (wchar_t)'\0';

  dw->d_hFindFile = FindFirstFileW(pw, &dw->d_fdw);
  free((zvoid *)pw);

  if (dw->d_hFindFile == INVALID_HANDLE_VALUE)
  {
    free((zvoid *)dw);
    return NULL;
  }

  dw->d_first = 1;
  return dw;
}

#endif

local zDIRSCAN *OpenDirScan(n)
ZCONST char *n;          /* directory to open */
/* Start searching for files in the MSDOS directory n */
{
  zDIRSCAN *d;          /* malloc'd return value */
  char *p;              /* malloc'd temporary string */
  char *q;

  if ((d = (zDIRSCAN *)malloc(sizeof(zDIRSCAN))) == NULL ||
      (p = malloc(strlen(n) + (2 + sizeof(wild_match_all)))) == NULL) {
    if (d != NULL) free((zvoid *)d);
    return NULL;
  }
  strcpy(p, n);
  q = p + strlen(p);
  if ((q - p) > 0 && MBSRCHR(p, ':') == (q - 1))
      *q++ = '.';
  if ((q - p) > 0 && MBSRCHR(p, '/') != (q - 1))
    *q++ = '/';
  strcpy(q, wild_match_all);

#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  OemToAnsi(p, p);
#endif
  d->d_hFindFile = FindFirstFile(p, &d->d_fd);
  free((zvoid *)p);

  if (d->d_hFindFile == INVALID_HANDLE_VALUE)
  {
    free((zvoid *)d);
    return NULL;
  }

  d->d_first = 1;
  return d;
}


#ifdef UNICODE_SUPPORT

local struct zdirscanw *GetNextDirEntryW(dw)
zDIRSCANW *dw;            /* directory stream to read from */
/* Return pointer to first or next directory entry, or NULL if end. */
{
  if (dw->d_first)
    dw->d_first = 0;
  else
  {
    if (!FindNextFileW(dw->d_hFindFile, &dw->d_fdw))
        return NULL;
  }
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  CharToOemW(dw->d_fdw.cFileName, dw->d_fdw.cFileName);
#endif
  return (struct zdirscanw *)dw;
}

#endif

local struct zdirscan *GetNextDirEntry(d)
zDIRSCAN *d;            /* directory stream to read from */
/* Return pointer to first or next directory entry, or NULL if end. */
{
  if (d->d_first)
    d->d_first = 0;
  else
  {
    if (!FindNextFile(d->d_hFindFile, &d->d_fd))
        return NULL;
  }
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  AnsiToOem(d->d_fd.cFileName, d->d_fd.cFileName);
#endif
  return (struct zdirscan *)d;
}

local void CloseDirScan(d)
zDIRSCAN *d;            /* directory stream to close */
{
  FindClose(d->d_hFindFile);
  free((zvoid *)d);
}

#ifdef UNICODE_SUPPORT

local void CloseDirScanW(dw)
zDIRSCANW *dw;         /* directory stream to close */
{
  FindClose(dw->d_hFindFile);
  free((zvoid *)dw);
}

#endif


#ifdef UNICODE_SUPPORT

local wchar_t *readdw(dw)
  zDIRSCANW *dw;         /* directory stream to read from */
/* Return a pointer to the next name in the directory stream dw, or NULL if
   no more entries or an error occurs. */
{
  struct zdirscanw *ew;

  do
    ew = GetNextDirEntryW(dw);
  while (ew &&
         ((!hidden_files && ew->d_fdw.dwFileAttributes & HIDD_SYS_BITS) ||
          (only_archive_set &&
           !(ew->d_fdw.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) &&
           !(ew->d_fdw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))));
  if (ew == NULL)
    return (wchar_t *) NULL;
  return ew->d_fdw.cFileName;
}

#endif

local char *readd(d)
zDIRSCAN *d;            /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct zdirscan *e;

  do
    e = GetNextDirEntry(d);
  while (e &&
         ((!hidden_files && e->d_fd.dwFileAttributes & HIDD_SYS_BITS) ||
          (only_archive_set &&
           !(e->d_fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) &&
           !(e->d_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))));
  /* When a wide character that is not supported by the current character
     set is found, FindFirstFile and FindNextFile return a "?" in that spot.
     A question mark is illegal in file names, so this flags that something
     needs to be done.  It seems the fix is to use the 8.3 name in
     this case, as that allows directory scans to continue.
   */
  if (e == NULL)
    return (char *) NULL;
  if (strchr(e->d_fd.cFileName, '?') && e->d_fd.cAlternateFileName) {
    /* Have '?' in name, assume wide character we can't handle is in
       the name and use short name if there is one.
    */
    return e->d_fd.cAlternateFileName;
  }
  return e->d_fd.cFileName;
}


#if 0
/* scan for the file in p and return Windows long name */
char *get_win32_longpath(p, n)
  char *p;               /* path to get short name path for */
  char **n;              /* pointer to name in returned path */
{
  char  *q;              /* return string */
  char  *c;
  int    is_dir = 0;
  char  *f;
  char  *fp;
  int    nr;
  int    fplen;
  int    fplen2;
  HANDLE d_hFindFile;
  WIN32_FIND_DATA d_fd;
  int slashes = 0;
  int returnslashes = 0;

  if (p == NULL)
    return NULL;

  /* count path components */
  for (f = p; *f; f++) {
    if (*f == '/' || *f == '\\') {
      slashes++;
    }
  }
  /* Ignore trailing slash */
  if (*p && (*(f - 1) == '/' || *(f - 1) == '\\'))
    slashes--;

  /* get the length of the full path */
  fplen = GetFullPathName(p, 0, NULL, NULL);

  if ((fp = malloc(fplen + 1)) == NULL) {
    return NULL;
  }
  /* get full path */
  fplen2 = GetFullPathName(p, fplen, fp, &f);
  if (fplen2 > fplen) {
    /* something changed */
    free(fp);
    return NULL;
  }
  c = fp + strlen(fp) - 1;
  if (*c == '\\' || *c == '/') {
    is_dir = 1;
    *c = '\0';
  }

  d_hFindFile = FindFirstFile(fp, &d_fd);
  free(fp);

  if (d_hFindFile == INVALID_HANDLE_VALUE)
  {
    return NULL;
  }
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  AnsiToOem(d->d_fd.cFileName, d->d_fd.cFileName);
#endif

  FindClose(d_hFindFile);

  if (d_fd.cFileName == NULL) {
    return NULL;
  }

  /* get the length of the full path */
  fplen = GetFullPathName(d_fd.cFileName, 0, NULL, NULL);

  if ((fp = malloc(fplen + 1)) == NULL) {
    return NULL;
  }
  /* get full path */
  fplen2 = GetFullPathName(d_fd.cFileName, fplen, fp, &f);
  if (fplen2 > fplen) {
    /* something changed */
    free(fp);
    return NULL;
  }

  /* characters from end to start of last component */
  nr = 0;

  /* find start of relative path we came in with */
  for (f = fp + strlen(fp); f != fp; f--) {
    if (*f == ':')
      break;
    if (*f == '/' || *f == '\\') {
      returnslashes++;
      /* convert \ to / */
      *f = '/';
      if (nr == 0)
        /* first slash from end */
        nr = strlen(fp) - (f - fp);
      if (returnslashes > slashes)
        break;
    }
    if (*f == '\\' && *(f + 1) == '\\')
      break;
  }
  if (f != fp)
    /* on slash in middle */
    f++;

  if ((q = malloc(strlen(f) + 2)) == NULL) {
    return NULL;
  }
  strcpy(q, f);
  *n = q + (strlen(q) - nr + 1);
  if (is_dir) {
    strcat(q, "/");
  }
  free(fp);

  return q;
}
#endif


#if 0
/* scan for the file in p and return Windows UTF-8 name */
char *get_win32_utf8path(p)
  char *p;               /* path to get utf-8 name for */
{
  char     *q;           /* return string */
  char     *r = NULL;
  int       is_dir = 0;
  char     *f;
  char     *fcp;
  char     *fp;
  wchar_t  *qw;
  char     *lastc = '\0';
  int       fplen;
  int       fplen2;
  int       ulen;
  int       ulenw;
  HANDLE    d_hFindFile;
  WIN32_FIND_DATAW d_fd;
  int pathslashes = 0;
  int componentslashes = 0;
  int slashes = 0;

  if (p == NULL)
    return NULL;

  /* count path components */
  for (f = p; *f; PREINCSTR(f)) {
    if (*f == '/' || *f == '\\') {
      slashes++;
    }
    lastc = f;
  }
  /* do not count trailing / */
  if (*lastc == '/' || *lastc == '\\') {
    is_dir = 1;
    slashes--;
  }

  /* Get the short path (as a bad long path could cause FindFirstFile to fail) */

  /* get the length of the short path */
  fplen = GetShortPathName(p, NULL, 0);

  if ((fp = malloc(fplen + 1)) == NULL) {
    return NULL;
  }
  /* get short path */
  fplen2 = GetShortPathName(p, fp, fplen);
  if (fplen2 > fplen) {
    /* something changed */
    free(fp);
    return NULL;
  }

  for (pathslashes = 0; pathslashes <= slashes; pathslashes++)
  {

    /* get component path */
    if ((fcp = malloc(fplen + 1)) == NULL) {
      return NULL;
    }
    strcpy(fcp, fp);
    componentslashes = 0;
    for (f = fcp; *f; PREINCSTR(f)) {
      if (*f == '/' || *f == '\\') {
        componentslashes++;
        if (componentslashes > pathslashes)
          break;
      }
      lastc = f;
    }
    *f = '\0';


    /* Get information for the file, including wide path */

    /* get length */
    ulenw = MultiByteToWideChar(
                CP_ACP,            /* ANSI code page */
                0,                 /* flags for character-type options */
                fcp,               /* string to convert */
                -1,                /* string length (-1 = NULL terminated) */
                NULL,              /* buffer */
                0 );               /* buffer length (0 = return length) */
    if (ulenw == 0) {
      /* failed */
      free(fcp);
      free(fp);
      return NULL;
    }
    ulenw++;
    /* get length in bytes */
    ulen = sizeof(wchar_t) * (ulenw + 1);
    if ((qw = (wchar_t *)malloc(ulen + 1)) == NULL) {
      free(fcp);
      free(fp);
      return NULL;
    }
    /* convert multibyte to wide */
    ulen = MultiByteToWideChar(
               CP_ACP,            /* ANSI code page */
               0,                 /* flags for character-type options */
               fcp,               /* string to convert */
               -1,                /* string length (-1 = NULL terminated) */
               qw,                /* buffer */
               ulenw);            /* buffer length (0 = return length) */
    if (ulen == 0) {
      /* failed */
      free(qw);
      free(fcp);
      free(fp);
      return 0;
    }

    d_hFindFile = FindFirstFileW(qw, &d_fd);
    /* If this Win32 platform does not support Unicode wide paths
       this returns INVALID_HANDLE_VALUE and the OS error is
       "No such file or directory".  We return NULL and go with
       the UTF-8 version of z->iname in f->uname.
     */
    free(qw);
    free(fcp);
    FindClose(d_hFindFile);

    if (d_hFindFile == INVALID_HANDLE_VALUE)
    {
      return NULL;
    }

    /* Get buffer length */
    ulen = WideCharToMultiByte(
                    CP_UTF8,        /* UTF-8 code page */
                    0,              /* flags */
                    d_fd.cFileName, /* string to convert */
                    -1,             /* input chars (-1 = NULL terminated) */
                    NULL,           /* buffer */
                    0,              /* size of buffer (0 = return needed size) */
                    NULL,           /* default char */
                    NULL);          /* used default char */
    if (ulen == 0) {
      /* failed */
      return NULL;
    }
    ulen += 2;
    if ((q = malloc(ulen + 1)) == NULL) {
      return NULL;
    }

    /* Convert the Unicode string to UTF-8 */
    if ((ulen = WideCharToMultiByte(
                    CP_UTF8,        /* UTF-8 code page */
                    0,              /* flags */
                    d_fd.cFileName, /* string to convert */
                    -1,             /* input chars (-1 = NULL terminated) */
                    q,              /* buffer */
                    ulen,           /* size of buffer (0 = return needed size) */
                    NULL,           /* default char */
                    NULL)) == 0)    /* used default char */
    {
      free(fp);
      free(q);
      return NULL;
    }

    if (r == NULL) {
      /* first one */
      r = q;
    } else {
      if ((r = realloc(r, strlen(r) + strlen(q) + 3)) == NULL) {
        free(fp);
        free(q);
        return NULL;
      }
      strcat(r, "/");
      strcat(r, q);
      free(q);
    }
  }

  free(fp);

  if (is_dir) {
    strcat(r, "/");
  }

  return r;
}
#endif


#define ONENAMELEN 255

/* whole is a pathname with wildcards, wildtail points somewhere in the  */
/* middle of it.  All wildcards to be expanded must come AFTER wildtail. */


#ifdef UNICODE_SUPPORT

wchar_t *local_to_wchar_string(local_string)
  char *local_string;       /* path to get utf-8 name for */
{
  wchar_t  *qw;
  int       ulen;
  int       ulenw;

  if (local_string == NULL)
    return NULL;

    /* get length */
    ulenw = MultiByteToWideChar(
                CP_ACP,            /* ANSI code page */
                0,                 /* flags for character-type options */
                local_string,      /* string to convert */
                -1,                /* string length (-1 = NULL terminated) */
                NULL,              /* buffer */
                0 );               /* buffer length (0 = return length) */
    if (ulenw == 0) {
      /* failed */
      return NULL;
    }
    ulenw++;
    /* get length in bytes */
    ulen = sizeof(wchar_t) * (ulenw + 1);
    if ((qw = (wchar_t *)malloc(ulen + 1)) == NULL) {
      return NULL;
    }
    /* convert multibyte to wide */
    ulen = MultiByteToWideChar(
               CP_ACP,            /* ANSI code page */
               0,                 /* flags for character-type options */
               local_string,      /* string to convert */
               -1,                /* string length (-1 = NULL terminated) */
               qw,                /* buffer */
               ulenw);            /* buffer length (0 = return length) */
    if (ulen == 0) {
      /* failed */
      free(qw);
      return NULL;
    }

  return qw;
}


wchar_t *utf8_to_wchar_string(utf8_string)
  char *utf8_string;       /* path to get utf-8 name for */
{
  wchar_t  *qw;
  int       ulen;
  int       ulenw;

  if (utf8_string == NULL)
    return NULL;

    /* get length */
    ulenw = MultiByteToWideChar(
                CP_UTF8,           /* UTF-8 code page */
                0,                 /* flags for character-type options */
                utf8_string,       /* string to convert */
                -1,                /* string length (-1 = NULL terminated) */
                NULL,              /* buffer */
                0 );               /* buffer length (0 = return length) */
    if (ulenw == 0) {
      /* failed */
      return NULL;
    }
    ulenw++;
    /* get length in bytes */
    ulen = sizeof(wchar_t) * (ulenw + 1);
    if ((qw = (wchar_t *)malloc(ulen + 1)) == NULL) {
      return NULL;
    }
    /* convert multibyte to wide */
    ulen = MultiByteToWideChar(
               CP_UTF8,           /* UTF-8 code page */
               0,                 /* flags for character-type options */
               utf8_string,       /* string to convert */
               -1,                /* string length (-1 = NULL terminated) */
               qw,                /* buffer */
               ulenw);            /* buffer length (0 = return length) */
    if (ulen == 0) {
      /* failed */
      free(qw);
      return NULL;
    }

  return qw;
}



/* Convert wchar_t string to utf8 using Windows calls
   so any characters needing more than one wchar_t are
   are handled by Windows */
char *wchar_to_utf8_string(wstring)
  wchar_t *wstring;
{
  char     *q;           /* return string */
  int       ulen;

  if (wstring == NULL)
    return NULL;

  /* Get buffer length */
  ulen = WideCharToMultiByte(
                  CP_UTF8,        /* UTF-8 code page */
                  0,              /* flags */
                  wstring,        /* string to convert */
                  -1,             /* input chars (-1 = NULL terminated) */
                  NULL,           /* buffer */
                  0,              /* size of buffer (0 = return needed size) */
                  NULL,           /* default char */
                  NULL);          /* used default char */
  if (ulen == 0) {
    /* failed */
    return NULL;
  }
  ulen += 2;
  if ((q = malloc(ulen + 1)) == NULL) {
    return NULL;
  }

  /* Convert the Unicode string to UTF-8 */
  if ((ulen = WideCharToMultiByte(
                  CP_UTF8,        /* UTF-8 code page */
                  0,              /* flags */
                  wstring,        /* string to convert */
                  -1,             /* input chars (-1 = NULL terminated) */
                  q,              /* buffer */
                  ulen,           /* size of buffer (0 = return needed size) */
                  NULL,           /* default char */
                  NULL)) == 0)    /* used default char */
  {
    free(q);
    return NULL;
  }

  return q;
}


local int wild_recursew(whole, wildtail)
  wchar_t *whole;
  wchar_t *wildtail;
{
    zDIRSCANW *dirw;
    wchar_t *subwild, *name, *newwhole = NULL, *glue = NULL, plug = 0, plug2;
    extent newlen;
    int amatch = 0, e = ZE_MISS;

    if (!isshexpw(wildtail)) {
        if (GetFileAttributesW(whole) != 0xFFFFFFFF) {    /* file exists? */
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
            CharToOemW(whole, whole);
#endif
            return procnamew(whole, 0);
        }
        else
            return ZE_MISS;                     /* woops, no wildcards! */
    }

    /* back up thru path components till existing dir found */
    do {
        name = wildtail + wcslen(wildtail) - 1;
        for (;;)
            if (name-- <= wildtail || *name == PATH_END) {
                subwild = name + 1;
                plug2 = *subwild;
                *subwild = 0;
                break;
            }
        if (glue)
            *glue = plug;
        glue = subwild;
        plug = plug2;
        dirw = OpenDirScanW(whole);
    } while (!dirw && subwild > wildtail);
    wildtail = subwild;                 /* skip past non-wild components */

    if ((subwild = wcschr(wildtail + 1, PATH_END)) != NULL) {
        /* this "+ 1" dodges the   ^^^ hole left by *glue == 0 */
        *(subwild++) = 0;               /* wildtail = one component pattern */
        newlen = wcslen(whole) + wcslen(subwild) + (ONENAMELEN + 2);
    } else
        newlen = wcslen(whole) + (ONENAMELEN + 1);
    if (!dirw || ((newwhole = malloc(newlen * sizeof(wchar_t))) == NULL)) {
        if (glue)
            *glue = plug;
        e = dirw ? ZE_MEM : ZE_MISS;
        goto ohforgetit;
    }
    wcscpy(newwhole, whole);
    newlen = wcslen(newwhole);
    if (glue)
        *glue = plug;                           /* repair damage to whole */
    if (!isshexpw(wildtail)) {
        e = ZE_MISS;                            /* non-wild name not found */
        goto ohforgetit;
    }

    while ((name = readdw(dirw)) != NULL) {
        if (wcscmp(name, L".") && wcscmp(name, L"..") &&
            MATCHW(wildtail, name, 0)) {
            wcscpy(newwhole + newlen, name);
            if (subwild) {
                name = newwhole + wcslen(newwhole);
                *(name++) = (wchar_t)PATH_END;
                wcscpy(name, subwild);
                e = wild_recursew(newwhole, name);
            } else
                e = procname_win32w(newwhole, 0, GetDirAttribsW(dirw));
            newwhole[newlen] = 0;
            if (e == ZE_OK)
                amatch = 1;
            else if (e != ZE_MISS)
                break;
        }
    }

  ohforgetit:
    if (dirw) CloseDirScanW(dirw);
    if (subwild) *--subwild = PATH_END;
    if (newwhole) free(newwhole);
    if (e == ZE_MISS && amatch)
        e = ZE_OK;
    return e;
}

#endif


local int wild_recurse(whole, wildtail)
  char *whole;
  char *wildtail;
{
    zDIRSCAN *dir;
    char *subwild, *name, *newwhole = NULL, *glue = NULL, plug = 0, plug2;
    extent newlen;
    int amatch = 0, e = ZE_MISS;

    if (!isshexp(wildtail)) {
        if (GetFileAttributes(whole) != 0xFFFFFFFF) {    /* file exists? */
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
            AnsiToOem(whole, whole);
#endif
            return procname(whole, 0);
        }
        else
            return ZE_MISS;                     /* woops, no wildcards! */
    }

    /* back up thru path components till existing dir found */
    do {
        name = wildtail + strlen(wildtail) - 1;
        for (;;)
            if (name-- <= wildtail || *name == PATH_END) {
                subwild = name + 1;
                plug2 = *subwild;
                *subwild = 0;
                break;
            }
        if (glue)
            *glue = plug;
        glue = subwild;
        plug = plug2;
        dir = OpenDirScan(whole);
    } while (!dir && subwild > wildtail);
    wildtail = subwild;                 /* skip past non-wild components */

    if ((subwild = MBSCHR(wildtail + 1, PATH_END)) != NULL) {
        /* this "+ 1" dodges the   ^^^ hole left by *glue == 0 */
        *(subwild++) = 0;               /* wildtail = one component pattern */
        newlen = strlen(whole) + strlen(subwild) + (ONENAMELEN + 2);
    } else
        newlen = strlen(whole) + (ONENAMELEN + 1);
    if (!dir || ((newwhole = malloc(newlen)) == NULL)) {
        if (glue)
            *glue = plug;
        e = dir ? ZE_MEM : ZE_MISS;
        goto ohforgetit;
    }
    strcpy(newwhole, whole);
    newlen = strlen(newwhole);
    if (glue)
        *glue = plug;                           /* repair damage to whole */
    if (!isshexp(wildtail)) {
        e = ZE_MISS;                            /* non-wild name not found */
        goto ohforgetit;
    }

    while ((name = readd(dir)) != NULL) {
        if (strcmp(name, ".") && strcmp(name, "..") &&
            MATCH(wildtail, name, 0)) {
            strcpy(newwhole + newlen, name);
            if (subwild) {
                name = newwhole + strlen(newwhole);
                *(name++) = PATH_END;
                strcpy(name, subwild);
                e = wild_recurse(newwhole, name);
            } else
                e = procname_win32(newwhole, 0, GetDirAttribs(dir));
            newwhole[newlen] = 0;
            if (e == ZE_OK)
                amatch = 1;
            else if (e != ZE_MISS)
                break;
        }
    }

  ohforgetit:
    if (dir) CloseDirScan(dir);
    if (subwild) *--subwild = PATH_END;
    if (newwhole) free(newwhole);
    if (e == ZE_MISS && amatch)
        e = ZE_OK;
    return e;
}


#ifdef UNICODE_SUPPORT
int has_win32_wide() {
  DWORD r;

  /* test if we have wide function support */

  /* check if already set */
  if (no_win32_wide != -1)
    return !no_win32_wide;

  /* assume we don't */
  no_win32_wide = 1;

  /* get attributes for this directory */
  r = GetFileAttributes(".");

  /* r should be 16 = FILE_ATTRIBUTE_DIRECTORY */
  if (r == FILE_ATTRIBUTE_DIRECTORY) {
    /* now see if it works for the wide version */
    r = GetFileAttributesW(L".");
    /* if this fails then we probably don't have wide functions */
    if (r == 0xFFFFFFFF) {
      /* error is probably "This function is only valid in Win32 mode." */
    } else if (r == FILE_ATTRIBUTE_DIRECTORY) {
      /* worked, so assume we have wide support */
      no_win32_wide = 0;
    }
  }

  return !no_win32_wide;
}
#endif


int wild(w)
  char *w;               /* path/pattern to match */
/* If not in exclude mode, expand the pattern based on the contents of the
   file system.  Return an error code in the ZE_ class. */
{
    char *p;             /* path */
    char *q;             /* diskless path */
    int e;               /* result */
#ifdef UNICODE_SUPPORT
    wchar_t *pw;         /* wide path */
    wchar_t *qw;         /* wide diskless path */
#endif

    if (volume_label == 1) {
      volume_label = 2;
      label = getVolumeLabel((w != NULL && isascii((uch)w[0]) && w[1] == ':')
                             ? to_up(w[0]) : '\0',
                             &label_time, &label_mode, &label_utim);
      if (label != NULL)
        (void)newname(label, 0, 0);
      if (w == NULL || (isascii((uch)w[0]) && w[1] == ':' && w[2] == '\0'))
        return ZE_OK;
      /* "zip -$ foo a:" can be used to force drive name */
    }
    /* special handling of stdin request */
    if (strcmp(w, "-") == 0)   /* if compressing stdin */
        return newname(w, 0, 0);

    /* Allocate and copy pattern, leaving room to add "." if needed */
    if ((p = malloc(strlen(w) + 2)) == NULL)
        return ZE_MEM;
    strcpy(p, w);

    /* Normalize path delimiter as '/' */
    for (q = p; *q; INCSTR(q))            /* use / consistently */
        if (*q == '\\')
            *q = '/';

#ifdef UNICODE_SUPPORT
    if (!no_win32_wide) {
      /* wide char version */
      pw = local_to_wchar_string(p);

      /* Separate the disk part of the path */
      if ((qw = wcschr(pw, ':')) != NULL) {
          if (wcschr(++qw, ':'))     /* sanity check for safety of wild_recurse */
              return ZE_MISS;
      } else
          qw = pw;

      /* Normalize bare disk names */
      if (qw > pw && !*qw)
          wcscpy(qw, L".");
    } else {
      /* multibyte version */
      /* Separate the disk part of the path */
      if ((q = MBSCHR(p, ':')) != NULL) {
          if (MBSCHR(++q, ':'))     /* sanity check for safety of wild_recurse */
              return ZE_MISS;
      } else
          q = p;

      /* Normalize bare disk names */
      if (q > p && !*q)
          strcpy(q, ".");
    }
#else
    /* multibyte version */
    /* Separate the disk part of the path */
    if ((q = MBSCHR(p, ':')) != NULL) {
        if (MBSCHR(++q, ':'))     /* sanity check for safety of wild_recurse */
            return ZE_MISS;
    } else
        q = p;

    /* Normalize bare disk names */
    if (q > p && !*q)
        strcpy(q, ".");
#endif

    /* Here we go */
#ifdef UNICODE_SUPPORT
    if (!no_win32_wide) {
      /* use wide Unicode directory scan */
      e = wild_recursew(pw, qw);

      free(pw);
    } else {
      /* use multibyte directory scan */
      e = wild_recurse(p, q);
    }
#else
    e = wild_recurse(p, q);
#endif
    free((zvoid *)p);
    return e;
}


local int procname_win32(n, caseflag, attribs)
  char *n;                /* name to process */
  int caseflag;           /* true to force case-sensitive match */
  DWORD attribs;
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  zDIRSCAN *d;          /* directory stream from OpenDirScan() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  z_stat s;             /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (attribs != INVALID_WIN32_FILE_ATTRIBS)
  {
    /* Avoid calling stat() for performance reasons when it is already known
       (from a previous directory scan) that the passed name corresponds to
       a "real existing" file.  The only information needed further down in
       this function is the distinction between directory entries and other
       (typically normal file) entries.  This distinction can be derived from
       the file's attributes that the directory lookup has already provided
       "for free".
     */
    s.st_mode = ((attribs & FILE_ATTRIBUTE_DIRECTORY) ? S_IFDIR : S_IFREG);
  }
  else if (LSSTAT(n, &s)
#ifdef __TURBOC__
           /* For this compiler, stat() succeeds on wild card names! */
           /* Unfortunately, this causes failure on names containing */
           /* square bracket characters, which are legal in win32.   */
           || isshexp(n)
#endif
          )
  {
#ifdef UNICODE_SUPPORT
    char *uname = NULL;
#endif
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->oname);
        m = 0;
      }
    }
#ifdef UNICODE_SUPPORT
    /* also check escaped Unicode names */
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (z->zuname) {
#ifdef WIN32
        /* It seems something is lost in going from a listed
           name from zip -su in a console window to using that
           name in a command line.  This kluge may fix it
           and just takes zuname, converts to oem (i.e.ouname),
           then converts it back which ends up not the same as
           started with.
         */
        uname = z->wuname;
#else
        uname = z->zuname;
#endif
        if (MATCH(p, uname, caseflag))
        {
          z->mark = pcount ? filter(uname, caseflag) : 1;
          if (verbose) {
              fprintf(mesg, "zip diagnostic: %scluding %s\n",
                 z->mark ? "in" : "ex", z->oname);
              fprintf(mesg, "     Escaped Unicode:  %s\n",
                 z->ouname);
          }
          m = 0;
        }
      }
    }
#endif
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  for (p = n; *p; INCSTR(p))    /* use / consistently */
    if (*p == '\\')
      *p = '/';
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add exclusions in directory recurse but ignored for single file */
    DWORD dwAttr;

    dwAttr = GetFileMode(n);

    if ((hidden_files ||
         !(dwAttr & FILE_ATTRIBUTE_HIDDEN || dwAttr & FILE_ATTRIBUTE_SYSTEM)) &&
        (!only_archive_set || (dwAttr & FILE_ATTRIBUTE_ARCHIVE)))
    {
      /* add or remove name of file */
      if ((m = newname(n, 0, caseflag)) != ZE_OK)
        return m;
    }
  } else {
    /* Add trailing / to the directory name */
    if ((p = (char *) malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    if (strcmp(n, ".") == 0 || strcmp(n, "/.") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (lastchar(p) != '/')
        strcpy(a, "/");
      if (dirnames && (m = newname(p, 1, caseflag)) != ZE_OK) {
        free((zvoid *)p);
        return m;
      }
    }
    /* recurse into directory */
    if (recurse && (d = OpenDirScan(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
          {
            CloseDirScan(d);
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcat(strcpy(a, p), e);
          if ((m = procname_win32(a, caseflag, GetDirAttribs(d)))
              != ZE_OK)         /* recurse on name */
          {
            if (m == ZE_MISS)
              zipwarn("name not matched: ", a);
            else
              ziperr(m, a);
          }
          free((zvoid *)a);
        }
      }
      CloseDirScan(d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}


#ifdef UNICODE_SUPPORT
local int procname_win32w(nw, caseflag, attribs)
  wchar_t *nw;             /* name to process */
  int caseflag;           /* true to force case-sensitive match */
  DWORD attribs;
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  wchar_t *aw;          /* path and name for recursion */
  zDIRSCANW *dw;        /* directory stream from OpenDirScan() */
  wchar_t *ew;          /* pointer to name from readd() */
  int m;                /* matched flag */
  wchar_t *pw;          /* path for recursion */
  zw_stat s;            /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (wcscmp(nw, L"-") == 0)   /* if compressing stdin */
    return newnamew(nw, 0, caseflag);
  else if (attribs != INVALID_WIN32_FILE_ATTRIBS)
  {
    /* Avoid calling stat() for performance reasons when it is already known
       (from a previous directory scan) that the passed name corresponds to
       a "real existing" file.  The only information needed further down in
       this function is the distinction between directory entries and other
       (typically normal file) entries.  This distinction can be derived from
       the file's attributes that the directory lookup has already provided
       "for free".
     */
    s.st_mode = ((attribs & FILE_ATTRIBUTE_DIRECTORY) ? S_IFDIR : S_IFREG);
  }
  else if (LSSTATW(nw, &s)
#ifdef __TURBOC__
           /* For this compiler, stat() succeeds on wild card names! */
           /* Unfortunately, this causes failure on names containing */
           /* square bracket characters, which are legal in win32.   */
           || isshexpw(nw)
#endif
          )
  {
    wchar_t *unamew = NULL;
    /* Not a file or directory--search for shell expression in zip file */
    pw = ex2inw(nw, 0, (int *)NULL);     /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCHW(pw, z->znamew, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->oname);
        m = 0;
      }
    }
    /* also check escaped Unicode names */
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (z->zuname) {
        unamew = z->znamew;
        if (MATCHW(pw, unamew, caseflag))
        {
          z->mark = pcount ? filter(z->iname, caseflag) : 1;
          if (verbose) {
              fprintf(mesg, "zip diagnostic: %scluding %s\n",
                 z->mark ? "in" : "ex", z->oname);
              fprintf(mesg, "     Escaped Unicode:  %s\n",
                 z->ouname);
          }
          m = 0;
        }
      }
    }
    free((zvoid *)pw);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  for (pw = nw; *pw; pw++)    /* use / consistently */
    if (*pw == (wchar_t)'\\')
      *pw = (wchar_t)'/';
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add exclusions in directory recurse but ignored for single file */
    DWORD dwAttr;

    dwAttr = GetFileModeW(nw);

    if ((hidden_files ||
         !(dwAttr & FILE_ATTRIBUTE_HIDDEN || dwAttr & FILE_ATTRIBUTE_SYSTEM)) &&
        (!only_archive_set || (dwAttr & FILE_ATTRIBUTE_ARCHIVE)))
    {
      /* add or remove name of file */
      if ((m = newnamew(nw, 0, caseflag)) != ZE_OK)
        return m;
    }
  } else {
    /* Add trailing / to the directory name */
    pw = (wchar_t *)malloc( (wcslen(nw)+2) * sizeof(wchar_t) );
    if (pw == NULL)
      return ZE_MEM;
    if (wcscmp(nw, L".") == 0 || wcscmp(nw, L"/.") == 0) {
      *pw = (wchar_t)'\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      wcscpy(pw, nw);
      aw = pw + wcslen(pw);
      if (pw[wcslen(pw) - 1] != (wchar_t)'/')
        wcscpy(aw, L"/");
      if (dirnames && (m = newnamew(pw, 1, caseflag)) != ZE_OK) {
        free((zvoid *)pw);
        return m;
      }
    }
    /* recurse into directory */
    if (recurse && (dw = OpenDirScanW(nw)) != NULL)
    {
      while ((ew = readdw(dw)) != NULL) {
        if (wcscmp(ew, L".") && wcscmp(ew, L".."))
        {
          if ((aw = malloc((wcslen(pw) + wcslen(ew) + 1) * sizeof(wchar_t))) == NULL)
          {
            CloseDirScanW(dw);
            free((zvoid *)pw);
            return ZE_MEM;
          }
          wcscat(wcscpy(aw, pw), ew);
          if ((m = procname_win32w(aw, caseflag, GetDirAttribsW(dw)))
              != ZE_OK)         /* recurse on name */
          {
            char *a;
            char *ad;

            a = wchar_to_local_string(aw);
            ad = local_to_display_string(a);

            if (m == ZE_MISS)
              zipwarn("name not matched: ", ad);
            else
              ziperr(m, a);
            free(ad);
            free(a);
          }
          free((zvoid *)aw);
        }
      }
      CloseDirScanW(dw);
    }
    free((zvoid *)pw);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}
#endif


#ifdef UNICODE_SUPPORT
int procnamew(nw, caseflag)
  wchar_t *nw;          /* name to process */
  int caseflag;         /* true to force case-sensitive match */
{
    return procname_win32w(nw, caseflag, INVALID_WIN32_FILE_ATTRIBS);
}
#endif

int procname(n, caseflag)
  char *n;             /* name to process */
  int caseflag;         /* true to force case-sensitive match */
{
    return procname_win32(n, caseflag, INVALID_WIN32_FILE_ATTRIBS);
}

char *ex2in(x, isdir, pdosflag)
  char *x;             /* external file name */
  int isdir;            /* input: x is a directory */
  int *pdosflag;        /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t;              /* shortened name */
  int dosflag;


  dosflag = dosify || IsFileSystemOldFAT(x);
  if (!dosify && use_longname_ea && (t = GetLongPathEA(x)) != NULL)
  {
    x = t;
    dosflag = 0;
  }

  /* Find starting point in name before doing malloc */
  /* Strip drive specification */
  t = *x && isascii((uch)*x) && *(x + 1) == ':' ? x + 2 : x;
  /* Strip "//host/share/" part of a UNC name */
  if ((!strncmp(x,"//",2) || !strncmp(x,"\\\\",2)) &&
      (x[2] != '\0' && x[2] != '/' && x[2] != '\\')) {
    n = x + 2;
    while (*n != '\0' && *n != '/' && *n != '\\')
      INCSTR(n);        /* strip host name */
    if (*n != '\0') {
      INCSTR(n);
      while (*n != '\0' && *n != '/' && *n != '\\')
        INCSTR(n);      /* strip `share' name */
    }
    if (*n != '\0')
      t = n + MB_CLEN(n);
  }
  /* Strip leading "/" to convert an absolute path into a relative path */
  while (*t == '/' || *t == '\\')
    t++;
  /* Strip leading "./" as well as drive letter */
  while (*t == '.' && (t[1] == '/' || t[1] == '\\'))
    t += 2;

  /* Make changes, if any, to the copied name (leave original intact) */
  for (n = t; *n; INCSTR(n))
    if (*n == '\\')
      *n = '/';

  if (!pathput)
    t = last(t, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;
  strcpy(n, t);

  if (dosify)
    msname(n);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  OemToAnsi(n, n);
#endif
  return n;
}

#ifdef UNICODE_SUPPORT
wchar_t *ex2inw(xw, isdir, pdosflag)
  wchar_t *xw;          /* external file name */
  int isdir;            /* input: x is a directory */
  int *pdosflag;        /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  wchar_t *nw;          /* internal file name (malloc'ed) */
  wchar_t *tw;          /* shortened name */
  int dosflag;


  dosflag = dosify || IsFileSystemOldFATW(xw);
  if (!dosify && use_longname_ea && (tw = GetLongPathEAW(xw)) != NULL)
  {
    xw = tw;
    dosflag = 0;
  }

  /* Find starting point in name before doing malloc */
  /* Strip drive specification */
  tw = *xw && iswascii(*xw) && *(xw + 1) == (wchar_t)':' ? xw + 2 : xw;
  /* Strip "//host/share/" part of a UNC name */
  if ((!wcsncmp(xw,L"//",2) || !wcsncmp(xw,L"\\\\",2)) &&
      (xw[2] != (wchar_t)'\0' && xw[2] != (wchar_t)'/' && xw[2] != (wchar_t)'\\')) {
    nw = xw + 2;
    while (*nw != (wchar_t)'\0' && *nw != (wchar_t)'/' && *nw != (wchar_t)'\\')
      nw++;        /* strip host name */
    if (*nw != (wchar_t)'\0') {
      nw++;
      while (*nw != (wchar_t)'\0' && *nw != (wchar_t)'/' && *nw != (wchar_t)'\\')
        nw++;      /* strip `share' name */
    }
    if (*nw != (wchar_t)'\0')
      tw = nw++;
  }
  /* Strip leading "/" to convert an absolute path into a relative path */
  while (*tw == (wchar_t)'/' || *tw == (wchar_t)'\\')
    tw++;
  /* Strip leading "./" as well as drive letter */
  while (*tw == (wchar_t)'.' && (tw[1] == (wchar_t)'/' || tw[1] == (wchar_t)'\\'))
    tw += 2;

  /* Make changes, if any, to the copied name (leave original intact) */
  for (nw = tw; *nw; nw++)
    if (*nw == '\\')
      *nw = '/';

  if (!pathput)
    tw = lastw(tw, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((nw = malloc((wcslen(tw) + 1) * sizeof(wchar_t))) == NULL)
    return NULL;
  wcscpy(nw, tw);

  if (dosify)
    msnamew(nw);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;
#if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  CharToAnsiW(nw, nw);
#endif
  return nw;
}
#endif


char *in2ex(n)
  char *n;             /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;             /* external file name */

  if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
    return NULL;
  strcpy(x, n);
# if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  AnsiToOem(x, x);
# endif
  return x;
}

#ifdef UNICODE_SUPPORT
wchar_t *in2exw(nw)
  wchar_t *nw;            /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  wchar_t *xw;            /* external file name */

  if ((xw = malloc((wcslen(nw) + 1 + PAD) * sizeof(wchar_t))) == NULL)
    return NULL;
  wcscpy(xw, nw);
# if defined(__RSXNT__)  /* RSXNT/EMX C rtl uses OEM charset */
  CharToOemW(xw, xw);
# endif
  return xw;
}
#endif


void stamp(f, d)
  char *f;                /* name of file to change */
  ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
#if defined(__TURBOC__) && !defined(__BORLANDC__)
  int h;                /* file handle */

  if ((h = open(f, 0)) != -1)
  {
    setftime(h, (struct ftime *)&d);
    close(h);
  }
#else /* !__TURBOC__ */

  struct utimbuf u;     /* argument for utime() */

  /* Convert DOS time to time_t format in u.actime and u.modtime */
  u.actime = u.modtime = dos2unixtime(d);

  /* Set updated and accessed times of f */
  utime(f, &u);
#endif /* ?__TURBOC__ */
}

ulg filetime(f, a, n, t)
  char *f;              /* name of file to get info on */
  ulg *a;               /* return value: file attributes */
  zoff_t *n;            /* return value: file size */
  iztimes *t;           /* return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  z_stat s;             /* results of zstat() */

  /* converted to malloc instead of using FNMAX - 11/8/04 EG */
  char *name;
  unsigned int len = strlen(f);
  int isstdin = !strcmp(f, "-");

  if (f == label) {
    if (a != NULL)
      *a = label_mode;
    if (n != NULL)
      *n = -2L; /* convention for a label name */
    if (t != NULL)
      t->atime = t->mtime = t->ctime = label_utim;
    return label_time;
  }
  if ((name = malloc(len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "filetime");
  }
  strcpy(name, f);
  if (MBSRCHR(name, '/') == (name + len - 1))
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  /* zip64 support 08/31/2003 R.Nausedat */
  if (isstdin) {
    if (zfstat(fileno(stdin), &s) != 0) {
      free(name);
      error("fstat(stdin)");
    }
    time((time_t *)&s.st_mtime);       /* some fstat()s return time zero */
  } else if (LSSTAT(name, &s) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(name);
    return 0;
  }

  if (a != NULL) {
#ifdef WIN32_OEM
    /* When creating DOS-like archives with OEM-charset names, only the
       standard FAT attributes should be used.
       (Note: On a Win32 system, the UNIX style attributes from stat()
              do not contain any additional information...)
     */
    *a = (isstdin ? 0L : (ulg)GetFileMode(name));
#else
    *a = ((ulg)s.st_mode << 16) | (isstdin ? 0L : (ulg)GetFileMode(name));
#endif
  }
  if (n != NULL)
    /* device return -1 */
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;
  }
  free(name);

  return unix2dostime((time_t *)&s.st_mtime);
}

#ifdef UNICODE_SUPPORT
ulg filetimew(fw, a, n, t)
  wchar_t *fw;          /* name of file to get info on */
  ulg *a;               /* return value: file attributes */
  zoff_t *n;            /* return value: file size */
  iztimes *t;           /* return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  zw_stat sw;           /* results of zstat() */

  /* converted to malloc instead of using FNMAX - 11/8/04 EG */
  wchar_t *namew;
  unsigned int len = wcslen(fw);
  int isstdin = !wcscmp(fw, L"-");
  wchar_t *labelw = local_to_wchar_string(label);

  if (labelw && wcscmp(fw, labelw) == 0) {
    if (a != NULL)
      *a = label_mode;
    if (n != NULL)
      *n = -2L; /* convention for a label name */
    if (t != NULL)
      t->atime = t->mtime = t->ctime = label_utim;
    return label_time;
  }
  if ((namew = malloc((len + 1) * sizeof(wchar_t))) == NULL) {
    ZIPERR(ZE_MEM, "filetime");
  }
  wcscpy(namew, fw);
  if (wcsrchr(namew, (wchar_t)'/') == (namew + len - 1))
    namew[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  /* zip64 support 08/31/2003 R.Nausedat */
  if (isstdin) {
    if (zwfstat(fileno(stdin), &sw) != 0) {
      free(namew);
      error("fstat(stdin)");
    }
    time((time_t *)&sw.st_mtime);       /* some fstat()s return time zero */
  } else if (LSSTATW(namew, &sw) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(namew);
    return 0;
  }

  if (a != NULL) {
#ifdef WIN32_OEM
    /* When creating DOS-like archives with OEM-charset names, only the
       standard FAT attributes should be used.
       (Note: On a Win32 system, the UNIX style attributes from stat()
              do not contain any additional information...)
     */
    *a = (isstdin ? 0L : (ulg)GetFileModeW(namew));
#else
    *a = ((ulg)sw.st_mode << 16) | (isstdin ? 0L : (ulg)GetFileModeW(namew));
#endif
  }
  if (n != NULL)
    /* device return -1 */
    *n = (sw.st_mode & S_IFMT) == S_IFREG ? sw.st_size : -1L;
  if (t != NULL) {
    t->atime = sw.st_atime;
    t->mtime = sw.st_mtime;
    t->ctime = sw.st_ctime;
  }
  free(namew);

  return unix2dostime((time_t *)&sw.st_mtime);
}
#endif



#ifdef NTSD_EAS

/* changed size, csize from size_t to ush 3/10/2005 EG */
local void GetSD(char *path, char **bufptr, ush *size,
                        char **cbufptr, ush *csize)
{
  unsigned char stackbuffer[NTSD_BUFFERSIZE];
  unsigned long bytes = NTSD_BUFFERSIZE;
  unsigned char *buffer = stackbuffer;
  unsigned char *DynBuffer = NULL;
  ulg cbytes;
  PEF_NTSD_L_HEADER pLocalHeader;
  PEF_NTSD_C_HEADER pCentralHeader;
  VOLUMECAPS VolumeCaps;

  /* check target volume capabilities */
  if (!ZipGetVolumeCaps(path, path, &VolumeCaps) ||
     !(VolumeCaps.dwFileSystemFlags & FS_PERSISTENT_ACLS)) {
    return;
  }

  VolumeCaps.bUsePrivileges = use_privileges;
  VolumeCaps.dwFileAttributes = 0;
  /* should set to file attributes, if possible */

  if (!SecurityGet(path, &VolumeCaps, buffer, (LPDWORD)&bytes)) {

    /* try to malloc the buffer if appropriate */
    if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        DynBuffer = malloc(bytes);
        if(DynBuffer == NULL) return;

        buffer = DynBuffer; /* switch to the new buffer and try again */

        if(!SecurityGet(path, &VolumeCaps, buffer, (LPDWORD)&bytes)) {
            free(DynBuffer);
            return;
        }

    } else {
        return; /* bail */
    }
  }

  /* # bytes to compress: compress type, CRC, data bytes */
  cbytes = (2 + 4 + EB_DEFLAT_EXTRA) + bytes;


  /* our two possible failure points.  don't allow trashing of any data
     if either fails - notice that *size and *csize don't get updated.
     *bufptr leaks if malloc() was used and *cbufptr alloc fails - this
     isn't relevant because it's probably indicative of a bigger problem. */

  if(*size)
    *bufptr = realloc(*bufptr, *size + EF_NTSD_L_LEN + cbytes);
  else
    *bufptr = malloc(EF_NTSD_L_LEN + cbytes);

  if(*csize)
    *cbufptr = realloc(*cbufptr, *csize + EF_NTSD_C_LEN);
  else
    *cbufptr = malloc(EF_NTSD_C_LEN);

  if(*bufptr == NULL || *cbufptr == NULL) {
    if(DynBuffer) free(DynBuffer);
    return;
  }

  /* local header */

  pLocalHeader = (PEF_NTSD_L_HEADER) (*bufptr + *size);

  cbytes = memcompress(((char *)pLocalHeader + EF_NTSD_L_LEN), cbytes,
                       (char *)buffer, bytes);

  if (cbytes > 0x7FFF) {
    sprintf(errbuf, "security info too large to store (%ld bytes), %d max", bytes, 0x7FFF);
    zipwarn(errbuf, "");
    zipwarn("security info not stored: ", path);
    if(DynBuffer) free(DynBuffer);
    return;
  }

  *size += EF_NTSD_L_LEN + (ush)cbytes;

  pLocalHeader->nID = EF_NTSD;
  pLocalHeader->nSize = (USHORT)(EF_NTSD_L_LEN - EB_HEADSIZE
                                 + cbytes);
  pLocalHeader->lSize = bytes; /* uncompressed size */
  pLocalHeader->Version = 0;

  /* central header */

  pCentralHeader = (PEF_NTSD_C_HEADER) (*cbufptr + *csize);
  *csize += EF_NTSD_C_LEN;

  pCentralHeader->nID = EF_NTSD;
  pCentralHeader->nSize = EF_NTSD_C_LEN - EB_HEADSIZE;  /* sbz */
  pCentralHeader->lSize = bytes;

  if (noisy) {
    sprintf(errbuf, " (%ld bytes security)", bytes);
    zipmessage_nl(errbuf, 0);
  }

  if(DynBuffer) free(DynBuffer);
}
#endif /* NTSD_EAS */


#ifdef USE_EF_UT_TIME

#define EB_L_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(3))
#define EB_C_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(1))

local int GetExtraTime(struct zlist far *z, iztimes *z_utim)
{
  char *eb_l_ptr;
  char *eb_c_ptr;
  ulg ultime;
  /* brain-dead IBM compiler defines time_t as "double", so we have to convert
   * it into unsigned long integer number...
   */

#ifdef IZ_CHECK_TZ
  if (!zp_tz_is_valid) return ZE_OK;    /* skip silently if no valid TZ info */
#endif

  if(z->ext)
    eb_l_ptr = realloc(z->extra, (z->ext + EB_L_UT_SIZE));
  else
    eb_l_ptr = malloc(EB_L_UT_SIZE);

  if (eb_l_ptr == NULL)
    return ZE_MEM;

  if(z->cext)
    eb_c_ptr = realloc(z->cextra, (z->cext + EB_C_UT_SIZE));
  else
    eb_c_ptr = malloc(EB_C_UT_SIZE);

  if (eb_c_ptr == NULL)
    return ZE_MEM;

  z->extra = eb_l_ptr;
  eb_l_ptr += z->ext;
  z->ext += EB_L_UT_SIZE;

  eb_l_ptr[0]  = 'U';
  eb_l_ptr[1]  = 'T';
  eb_l_ptr[2]  = EB_UT_LEN(3);          /* length of data part of e.f. */
  eb_l_ptr[3]  = 0;
  eb_l_ptr[4]  = EB_UT_FL_MTIME | EB_UT_FL_ATIME | EB_UT_FL_CTIME;
  ultime = (ulg)z_utim->mtime;
  eb_l_ptr[5]  = (char)(ultime);
  eb_l_ptr[6]  = (char)(ultime >> 8);
  eb_l_ptr[7]  = (char)(ultime >> 16);
  eb_l_ptr[8]  = (char)(ultime >> 24);
  ultime = (ulg)z_utim->atime;
  eb_l_ptr[9]  = (char)(ultime);
  eb_l_ptr[10] = (char)(ultime >> 8);
  eb_l_ptr[11] = (char)(ultime >> 16);
  eb_l_ptr[12] = (char)(ultime >> 24);
  ultime = (ulg)z_utim->ctime;
  eb_l_ptr[13] = (char)(ultime);
  eb_l_ptr[14] = (char)(ultime >> 8);
  eb_l_ptr[15] = (char)(ultime >> 16);
  eb_l_ptr[16] = (char)(ultime >> 24);

  z->cextra = eb_c_ptr;
  eb_c_ptr += z->cext;
  z->cext += EB_C_UT_SIZE;

  memcpy(eb_c_ptr, eb_l_ptr, EB_C_UT_SIZE);
  eb_c_ptr[EB_LEN] = EB_UT_LEN(1);

  return ZE_OK;
}

#endif /* USE_EF_UT_TIME */



int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
  /* create extra field and change z->att if desired */
{

#ifdef NTSD_EAS
  if(ZipIsWinNT()) {
    /* store SECURITY_DECRIPTOR data in local header,
       and size only in central headers */
    GetSD(z->name, &z->extra, &z->ext, &z->cextra, &z->cext);
  }
#endif /* NTSD_EAS */

#ifdef USE_EF_UT_TIME
  /* store extended time stamps in both headers */
  return GetExtraTime(z, z_utim);
#else /* !USE_EF_UT_TIME */
  return ZE_OK;
#endif /* ?USE_EF_UT_TIME */
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
   For VMS, d must be in format [x.y]z.dir;1  (not [x.y.z]).
 */
{
    return rmdir(d);
}

#endif /* !UTIL */
