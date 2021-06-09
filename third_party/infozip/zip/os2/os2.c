/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#include "zip.h"

#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

#include <time.h>
#if defined(__IBMC__) || defined(MSC)
#include <direct.h>
#endif

/* Extra malloc() space in names for cutpath() */
#define PAD 0
#define PATH_END '/'


#include "os2zip.h"

/* Library functions not in (most) header files */

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Local functions */
local char *readd OF((DIR *));


local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

int wild(w)
char *w;                /* path/pattern to match */
/* If not in exclude mode, expand the pattern based on the contents of the
   file system.  Return an error code in the ZE_ class. */
{
  DIR *d;               /* stream for reading directory */
  char *e;              /* name found in directory */
  int r;                /* temporary variable */
  char *n;              /* constructed name from directory */
  int f;                /* true if there was a match */
  char *a;              /* alloc'ed space for name */
  char *p;              /* path */
  char *q;              /* name */
  char v[5];            /* space for device current directory */

  if (volume_label == 1) {
    volume_label = 2;
    label = getVolumeLabel((w != NULL && w[1] == ':') ? to_up(w[0]) : '\0',
                           &label_time, &label_mode, &label_utim);
    if (label != NULL) {
       newname(label, 0, 0);
    }
    if (w == NULL || (w[1] == ':' && w[2] == '\0')) return ZE_OK;
    /* "zip -$ foo a:" can be used to force drive name */
  }

  if (w == NULL)
    return ZE_OK;

  /* special handling of stdin request */
  if (strcmp(w, "-") == 0)   /* if compressing stdin */
    return newname(w, 0, 0);

  /* Allocate and copy pattern */
  if ((p = a = malloc(strlen(w) + 1)) == NULL)
    return ZE_MEM;
  strcpy(p, w);

  /* catch special case: treat "*.*" as "*" for DOS-impaired people */
  r = strlen(p);
  if (strcmp(p + r - 3, "*.*") == 0)
    p[r - 2] = '\0';

  /* Normalize path delimiter as '/'. */
  for (q = p; *q; q++)                  /* use / consistently */
    if (*q == '\\')
      *q = '/';

  /* Only name can have special matching characters */
  if ((q = isshexp(p)) != NULL &&
      (strrchr(q, '/') != NULL || strrchr(q, ':') != NULL))
  {
    free((zvoid *)a);
    return ZE_PARMS;
  }

  /* Separate path and name into p and q */
  if ((q = strrchr(p, '/')) != NULL && (q == p || q[-1] != ':'))
  {
    *q++ = '\0';                        /* path/name -> path, name */
    if (*p == '\0')                     /* path is just / */
      p = strcpy(v, "/.");
  }
  else if ((q = strrchr(p, ':')) != NULL)
  {                                     /* has device and no or root path */
    *q++ = '\0';
    p = strcat(strcpy(v, p), ":");      /* copy device as path */
    if (*q == '/')                      /* -> device:/., name */
    {
      strcat(p, "/");
      q++;
    }
    strcat(p, ".");
  }
  else if (recurse && (strcmp(p, ".") == 0 ||  strcmp(p, "..") == 0))
  {                                    /* current or parent directory */
    /* I can't understand Mark's code so I am adding a hack here to get
     * "zip -r foo ." to work. Allow the dubious "zip -r foo .." but
     * reject "zip -rm foo ..".
     */
    if (dispose && strcmp(p, "..") == 0)
       ziperr(ZE_PARMS, "cannot remove parent directory");
    q = "*";
  }
  else                                  /* no path or device */
  {
    q = p;
    p = strcpy(v, ".");
  }
  if (recurse && *q == '\0') {
    q = "*";
  }
  /* Search that level for matching names */
  if ((d = opendir(p)) == NULL)
  {
    free((zvoid *)a);
    return ZE_MISS;
  }
  if ((r = strlen(p)) > 1 &&
      (strcmp(p + r - 2, ":.") == 0 || strcmp(p + r - 2, "/.") == 0))
    *(p + r - 1) = '\0';
  f = 0;
  while ((e = readd(d)) != NULL) {
    if (strcmp(e, ".") && strcmp(e, "..") && MATCH(q, e, 0))
    {
      f = 1;
      if (strcmp(p, ".") == 0) {                /* path is . */
        r = procname(e, 0);                     /* name is name */
        if (r) {
           f = 0;
           break;
        }
      } else
      {
        if ((n = malloc(strlen(p) + strlen(e) + 2)) == NULL)
        {
          free((zvoid *)a);
          closedir(d);
          return ZE_MEM;
        }
        n = strcpy(n, p);
        if (n[r = strlen(n) - 1] != '/' && n[r] != ':')
          strcat(n, "/");
        r = procname(strcat(n, e), 0);          /* name is path/name */
        free((zvoid *)n);
        if (r) {
          f = 0;
          break;
        }
      }
    }
  }
  closedir(d);

  /* Done */
  free((zvoid *)a);
  return f ? ZE_OK : ZE_MISS;
}

int procname(n, caseflag)
char *n;                /* name to process */
int caseflag;           /* true to force case-sensitive match */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  DIR *d;               /* directory stream from opendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (n == NULL)        /* volume_label request in freshen|delete mode ?? */
    return ZE_OK;

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (LSSTAT(n, &s)
#if defined(__TURBOC__) || defined(__WATCOMC__)
           /* For these 2 compilers, stat() succeeds on wild card names! */
           || isshexp(n)
#endif
          )
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  for (p = n; *p; p++)          /* use / consistently */
    if (*p == '\\')
      *p = '/';
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  } else {
    /* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    if (strcmp(n, ".") == 0 || strcmp(n, "/.") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (a[-1] != '/')
        strcpy(a, "/");
      if (dirnames && (m = newname(p, 1, caseflag)) != ZE_OK) {
        free((zvoid *)p);
        return m;
      }
    }
    /* recurse into directory */
    if (recurse && (d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
          {
            closedir(d);
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcat(strcpy(a, p), e);
          if ((m = procname(a, caseflag)) != ZE_OK)   /* recurse on name */
          {
            if (m == ZE_MISS)
              zipwarn("name not matched: ", a);
            else
              ziperr(m, a);
          }
          free((zvoid *)a);
        }
      }
      closedir(d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}

char *ex2in(x, isdir, pdosflag)
char *x;                /* external file name */
int isdir;              /* input: x is a directory */
int *pdosflag;          /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t;              /* shortened name */
  int dosflag;

  dosflag = dosify || IsFileSystemFAT(x) || (x == label);
  if (!dosify && use_longname_ea && (t = GetLongPathEA(x)) != NULL)
  {
    x = t;
    dosflag = 0;
  }

  /* Find starting point in name before doing malloc */
  /* Strip drive specification */
  t = *x && *(x + 1) == ':' ? x + 2 : x;
  /* Strip "//host/share/" part of a UNC name */
  if ((!strncmp(x,"//",2) || !strncmp(x,"\\\\",2)) &&
      (x[2] != '\0' && x[2] != '/' && x[2] != '\\')) {
    n = x + 2;
    while (*n != '\0' && *n != '/' && *n != '\\')
      n++;              /* strip host name */
    if (*n != '\0') {
      n++;
      while (*n != '\0' && *n != '/' && *n != '\\')
        n++;            /* strip `share' name */
    }
    if (*n != '\0')
      t = n + 1;
  }
  /* Strip leading "/" to convert an absolute path into a relative path */
  while (*t == '/' || *t == '\\')
    t++;
  /* Strip leading "./" as well as drive letter */
  while (*t == '.' && (t[1] == '/' || t[1] == '\\'))
    t += 2;

  /* Make changes, if any, to the copied name (leave original intact) */
  for (n = t; *n; n++)
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
  return n;
}


char *in2ex(n)
char *n;                /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */

  if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
    return NULL;
  strcpy(x, n);

  return x;
}


void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  SetFileTime(f, d);
}

ulg filetime(f, a, n, t)
char *f;                /* name of file to get info on */
ulg *a;                 /* return value: file attributes */
long *n;                /* return value: file size */
iztimes *t;             /* return value: access, modific. and creation times */
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
  struct stat s;        /* results of stat() */
  char *name;
  ulg r;
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
  if (name[len - 1] == '/')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  if (isstdin) {
    /* it is common for some PC based compilers to
       fail with fstat() on devices or pipes */
    if (fstat(fileno(stdin), &s) != 0) {
      s.st_mode = S_IFREG; s.st_size = -1L;
    }
    time(&s.st_ctime);
    s.st_atime = s.st_mtime = s.st_ctime;
  } else if (LSSTAT(name, &s) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(name);
    return 0;
  }

  if (a != NULL) {
    *a = ((ulg)s.st_mode << 16) | (isstdin ? 0L : (ulg)GetFileMode(name));
  }
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
#ifdef __WATCOMC__
  /* of course, Watcom always has to make an exception */
  if (s.st_atime == 312764400)
    s.st_atime = s.st_mtime;
  if (s.st_ctime == 312764400)
    s.st_ctime = s.st_mtime;
#endif
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;
  }

  r = GetFileTime(name);
  free(name);

  return r;
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
 */
{
    return rmdir(d);
}


#if defined MY_ZCALLOC /* Special zcalloc function for MEMORY16 (MSDOS/OS2) */

#ifdef MSC  /* Microsoft C */

zvoid far *zcalloc (unsigned items, unsigned size)
{
    return (zvoid far *)halloc((long)items, size);
}

zvoid zcfree (zvoid far *ptr)
{
    hfree((void huge *)ptr);
}

#endif /* MSC */

#endif /* MY_ZCALLOC */

#endif /* !UTIL */
