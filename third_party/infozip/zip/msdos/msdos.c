/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#include "zip.h"

#ifndef UTIL    /* little or no material in this file is used by UTIL */

#include <dos.h>
#include <time.h>


#if defined(__GO32__) || defined(__TURBOC__)
#  include <dir.h> /* prototypes of find*() */
   typedef struct ffblk   ff_dir;
#  define FATTR (hidden_files ? FA_HIDDEN+FA_SYSTEM+FA_DIREC : FA_DIREC)
#  define FFIRST(n,d,a)   findfirst(n,(struct ffblk *)d,a)
#  define FNEXT(d)        findnext((struct ffblk *)d)
#  if (defined(__TURBOC__) || (defined(__DJGPP__) && (__DJGPP__ >=2)))
#    if (defined(__DJGPP__) && (__DJGPP__ == 2) && (__DJGPP_MINOR__ == 0))
#      include <libc/dosio.h>
#    endif
#    define GetFileMode(name) _chmod(name, 0)
#    define SetFileMode(name, attr) _chmod(name, 1, attr)
#  else /* DJGPP v1.x */
#    define GetFileMode(name) bdosptr(0x43, (name), 0)
#  endif
#endif /* __GO32__ || __TURBOC__ */

#if defined(MSC) || defined(__WATCOMC__)
   typedef struct find_t  ff_dir;
#  define FATTR (hidden_files ? _A_HIDDEN+_A_SYSTEM+_A_SUBDIR : _A_SUBDIR)
#  ifndef FA_LABEL
#    define FA_LABEL      _A_VOLID
#  endif
#  define FFIRST(n,d,a)   _dos_findfirst(n,a,(struct find_t *)d)
#  define FNEXT(d)        _dos_findnext((struct find_t *)d)
#  define ff_name         name
#  define ff_fdate        wr_date
#  define ff_ftime        wr_time
#  define ff_attrib       attrib
#endif /* MSC || __WATCOMC__ */

#ifdef __EMX__
#  ifdef EMX_OBSOLETE           /* emx 0.9b or earlier */
#    define size_t xxx_size_t
#    define wchar_t xxx_wchar_t
#    define tm xxx_tm
#    include <sys/emx.h>
#    undef size_t
#    undef wchar_t
#    undef tm
#  else /* !EMX_OBSOLETE */     /* emx 0.9c or newer */
#    include <emx/syscalls.h>
#  endif /* ?EMX_OBSOLETE */
   typedef struct _find   ff_dir;
#  define FATTR (hidden_files ? _A_HIDDEN+_A_SYSTEM+_A_SUBDIR : _A_SUBDIR)
#  define FA_LABEL        _A_VOLID
#  define FFIRST(n,d,a)   __findfirst(n,a,d)
#  define FNEXT(d)        __findnext(d)
#  define ff_name         name
#  define ff_fdate        date
#  define ff_ftime        time
#  define ff_attrib       attr
#  define GetFileMode(name) __chmod(name, 0, 0)
#  define SetFileMode(name, attr) __chmod(name, 1, attr)
#endif /* __EMX__ */

#ifndef SetFileMode
#  define SetFileMode(name, attr) _dos_setfileattr(name, attr)
#endif


#define PAD  0
#define PATH_END '/'

/* Library functions not in (most) header files */
int rmdir OF((const char *));
int utime OF((char *, ztimbuf *));

/* Local functions */
#ifndef GetFileMode
int GetFileMode OF((char *name));
#endif /* !GetFileMode */

local int  initDirSearch OF((char *name, ff_dir *ff_context_p));
local char *getVolumeLabel OF((int, ulg *, ulg *, time_t *));
local int  wild_recurse OF((char *, char *));
local int procname_dos OF((char *n, int caseflag, unsigned attribs));
local int is_running_on_windows OF((void));

#define MSDOS_INVALID_ATTR      0xFF
#define getDirEntryAttr(d)      ((d)->ff_attrib)

/* Module level variables */
extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Module level constants */
local ZCONST char wild_match_all[] = "*.*";


#ifndef GetFileMode
int GetFileMode(char *name)
{
  unsigned int attr = 0;
  return (_dos_getfileattr(name, &attr) ? -1 : attr);
}
#endif /* !GetFileMode */

local int initDirSearch(name, ff_context_p)
  char *name;                   /* name of directory to scan */
  ff_dir *ff_context_p;         /* pointer to FFIRST/FNEXT context structure */
{
  int r;                        /* FFIRST return value */
  char *p, *q;                  /* temporary copy of name, and aux pointer */

  if ((p = malloc(strlen(name) + (2 + sizeof(wild_match_all)))) == NULL)
    return ZE_MEM;

  strcpy(p, name);
  q = p + strlen(p);
  if (q[-1] == ':')
    *q++ = '.';
  if ((q - p) > 0 && *(q - 1) != '/')
    *q++ = '/';
  strcpy(q, wild_match_all);
  r = FFIRST(p, ff_context_p, FATTR);
  free((zvoid *)p);

  return (r ? ZE_MISS : ZE_OK);
}

local char *getVolumeLabel(drive, vtime, vmode, vutim)
  int drive;    /* drive name: 'A' .. 'Z' or '\0' for current drive */
  ulg *vtime;   /* volume label creation time (DOS format) */
  ulg *vmode;   /* volume label file mode */
  time_t *vutim;/* volume label creation time (UNIX format) */

/* If a volume label exists for the given drive, return its name and
   set its time and mode. The returned name must be static data. */
{
  static char vol[14];
  ff_dir d;
  char *p;

  if (drive) {
    vol[0] = (char)drive;
    strcpy(vol+1, ":/");
  } else {
    strcpy(vol, "/");
  }
  strcat(vol, wild_match_all);
  if (FFIRST(vol, &d, FA_LABEL) == 0) {
    strncpy(vol, d.ff_name, sizeof(vol)-1);
    vol[sizeof(vol)-1] = '\0';   /* just in case */
    if ((p = strchr(vol, '.')) != NULL) /* remove dot, though PKZIP doesn't */
      strcpy(p, p + 1);
    *vtime = ((ulg)d.ff_fdate << 16) | ((ulg)d.ff_ftime & 0xffff);
    *vmode = (ulg)d.ff_attrib;
    *vutim = dos2unixtime(*vtime);
    return vol;
  }
  return NULL;
}


#ifdef MSDOS16
#define ONENAMELEN 12   /* no 16-bit compilers supports LFN */
#else
#define ONENAMELEN 255
#endif

/* whole is a pathname with wildcards, wildtail points somewhere in the  */
/* middle of it.  All wildcards to be expanded must come AFTER wildtail. */

local int wild_recurse(whole, wildtail)
char *whole;
char *wildtail;
{
    ff_dir dir;
    char *subwild, *name, *newwhole = NULL, *glue = NULL, plug = 0, plug2;
    ush newlen, amatch = 0;
    int e = ZE_MISS;

    if (!isshexp(wildtail)) {
        struct stat s;                          /* dummy buffer for stat() */

        if (!LSSTAT(whole, &s))                 /* file exists ? */
            return procname(whole, 0);
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
        e = initDirSearch(whole, &dir);
    } while (e == ZE_MISS && subwild > wildtail);
    wildtail = subwild;                 /* skip past non-wild components */
    if (e != ZE_OK) {
        if (glue)
            *glue = plug;
        goto ohforgetit;
    }
    subwild = strchr(wildtail + 1, PATH_END);
    /* this "+ 1" dodges the  ^^^ hole left by *glue == 0 */
    if (subwild != NULL) {
        *(subwild++) = 0;               /* wildtail = one component pattern */
        newlen = strlen(whole) + strlen(subwild) + (ONENAMELEN + 2);
    } else
        newlen = strlen(whole) + (ONENAMELEN + 1);
    if ((newwhole = malloc(newlen)) == NULL) {
        if (glue)
            *glue = plug;
        e = ZE_MEM;
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

    do {
        if (strcmp(dir.ff_name, ".") && strcmp(dir.ff_name, "..")
                                  && MATCH(wildtail, dir.ff_name, 0)) {
            strcpy(newwhole + newlen, dir.ff_name);
            if (subwild) {
                name = newwhole + strlen(newwhole);
                *(name++) = PATH_END;
                strcpy(name, subwild);
                e = wild_recurse(newwhole, name);
            } else
                e = procname_dos(newwhole, 0, getDirEntryAttr(&dir));
            newwhole[newlen] = 0;
            if (e == ZE_OK)
                amatch = 1;
            else if (e != ZE_MISS)
                break;
        }
    } while (FNEXT(&dir) == 0);

  ohforgetit:
    if (subwild)
        *--subwild = PATH_END;
    if (newwhole)
        free(newwhole);
    if (e == ZE_MISS && amatch)
        e = ZE_OK;
    return e;
}

int wild(w)
char *w;                /* path/pattern to match */
/* If not in exclude mode, expand the pattern based on the contents of the
   file system.  Return an error code in the ZE_ class. */
{
    char *p;            /* path */
    char *q;            /* diskless path */
    int e;              /* result */

    if (volume_label == 1) {
      volume_label = 2;
      label = getVolumeLabel((w != NULL && w[1] == ':') ? to_up(w[0]) : '\0',
                             &label_time, &label_mode, &label_utim);
      if (label != NULL)
        (void)newname(label, 0, 0);
      if (w == NULL || (w[1] == ':' && w[2] == '\0')) return ZE_OK;
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
    for (q = p; *q; q++)                  /* use / consistently */
        if (*q == '\\')
            *q = '/';

    /* Separate the disk part of the path */
    q = strchr(p, ':');
    if (q != NULL) {
        if (strchr(++q, ':'))     /* sanity check for safety of wild_recurse */
            return ZE_MISS;
    } else
        q = p;

    /* Normalize bare disk names */
    if (q > p && !*q)
        strcpy(q, ".");

    /* Here we go */
    e = wild_recurse(p, q);
    free((zvoid *)p);
    return e;
}

local int procname_dos(n, caseflag, attribs)
char *n;                /* name to process */
int caseflag;           /* true to force case-sensitive match */
unsigned attribs;       /* file attributes, if available */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  ff_dir *d;            /* control structure for FFIRST/FNEXT */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  int ff_status;        /* return value of FFIRST/FNEXT */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (n == NULL)        /* volume_label request in freshen|delete mode ?? */
    return ZE_OK;

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (*n == '\0') return ZE_MISS;
  else if (attribs != MSDOS_INVALID_ATTR)
  {
    /* Avoid calling stat() for performance reasons when it is already known
       (from a previous directory scan) that the passed name corresponds to
       a "real existing" file.  The only information needed further down in
       this function is the distinction between directory entries and other
       (typically normal file) entries.  This distinction can be derived from
       the file's attributes that the directory lookup has already provided
       "for free".
     */
    s.st_mode = ((attribs & MSDOS_DIR_ATTR) ? S_IFDIR : S_IFREG);
  }
  else if (LSSTAT(n, &s)
#ifdef __TURBOC__
           /* For this compiler, stat() succeeds on wild card names! */
           || isshexp(n)
#endif
          )
  {
    /* Not a file or directory--search for shell expression in zip file */
    if (caseflag) {
      p = malloc(strlen(n) + 1);
      if (p != NULL)
        strcpy(p, n);
    } else
      p = ex2in(n, 0, (int *)NULL);     /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (z->mark) z->dosflag = 1;    /* force DOS attribs for incl. names */
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
    if (recurse)
    {
      if ((d = malloc(sizeof(ff_dir))) == NULL ||
          (m = initDirSearch(n, d)) == ZE_MEM)
      {
        if (d != NULL)
          free((zvoid *)d);
        free((zvoid *)p);
        return ZE_MEM;
      }
      for (e = d->ff_name, ff_status = m;
           ff_status == 0;
           ff_status = FNEXT(d))
      {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
          {
            free((zvoid *)d);
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcat(strcpy(a, p), e);
          if ((m = procname_dos(a, caseflag, getDirEntryAttr(d)))
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
      free((zvoid *)d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}

int procname(n, caseflag)
char *n;                /* name to process */
int caseflag;           /* true to force case-sensitive match */
{
  return procname_dos(n, caseflag, MSDOS_INVALID_ATTR);
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

  dosflag = 1;

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
  /* Skip leading "./" as well */
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

  if (isdir == 42) return n;      /* avoid warning on unused variable */

  if (dosify)
    msname(n);
  else
#if defined(__DJGPP__) && __DJGPP__ >= 2
    if (_USE_LFN == 0)
#endif
      strlwr(n);
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
#if defined(__TURBOC__) || defined(__GO32__)
  int h;                /* file handle */

  if ((h = open(f, 0)) != -1)
  {
    setftime(h, (struct ftime *)(void *)&d);
    close(h);
  }
#else /* !__TURBOC__ && !__GO32__ */
  ztimbuf u;            /* argument for utime() */

  /* Convert DOS time to time_t format in u.actime and u.modtime */
  u.actime = u.modtime = dos2unixtime(d);

  /* Set updated and accessed times of f */
  utime(f, &u);
#endif /* ?(__TURBOC__ || __GO32__) */
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
  /* convert FNMAX to malloc - 11/8/04 EG */
  char *name;
  int len = strlen(f);
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
    if (fstat(fileno(stdin), &s) != 0) {
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
    *a = ((ulg)s.st_mode << 16) | (isstdin ? 0L : (ulg)GetFileMode(name));
#if (S_IFREG != 0x8000)
    /* kludge to work around non-standard S_IFREG flag used in DJGPP V2.x */
    if ((s.st_mode & S_IFMT) == S_IFREG) *a |= 0x80000000L;
#endif
  }
  free(name);
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;
  }

  return unix2dostime((time_t *)&s.st_mtime);
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
 */
{
    return rmdir(d);
}

int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
  /* create extra field and change z->att if desired */
{
#ifdef USE_EF_UT_TIME
#ifdef IZ_CHECK_TZ
  if (!zp_tz_is_valid) return ZE_OK;    /* skip silently if no valid TZ info */
#endif

  if ((z->extra = (char *)malloc(EB_HEADSIZE+EB_UT_LEN(1))) == NULL)
    return ZE_MEM;

  z->extra[0]  = 'U';
  z->extra[1]  = 'T';
  z->extra[2]  = EB_UT_LEN(1);          /* length of data part of e.f. */
  z->extra[3]  = 0;
  z->extra[4]  = EB_UT_FL_MTIME;
  z->extra[5]  = (char)(z_utim->mtime);
  z->extra[6]  = (char)(z_utim->mtime >> 8);
  z->extra[7]  = (char)(z_utim->mtime >> 16);
  z->extra[8]  = (char)(z_utim->mtime >> 24);

  z->cext = z->ext = (EB_HEADSIZE+EB_UT_LEN(1));
  z->cextra = z->extra;

  return ZE_OK;
#else /* !USE_EF_UT_TIME */
  return (int)(z-z);
#endif /* ?USE_EF_UT_TIME */
}


#ifdef MY_ZCALLOC       /* Special zcalloc function for MEMORY16 (MSDOS/OS2) */

#if defined(__TURBOC__) && !defined(OS2)
/* Small and medium model are for now limited to near allocation with
 * reduced MAX_WBITS and MAX_MEM_LEVEL
 */

/* Turbo C malloc() does not allow dynamic allocation of 64K bytes
 * and farmalloc(64K) returns a pointer with an offset of 8, so we
 * must fix the pointer. Warning: the pointer must be put back to its
 * original form in order to free it, use zcfree().
 */

#define MAX_PTR 10
/* 10*64K = 640K */

local int next_ptr = 0;

typedef struct ptr_table_s {
    zvoid far *org_ptr;
    zvoid far *new_ptr;
} ptr_table;

local ptr_table table[MAX_PTR];
/* This table is used to remember the original form of pointers
 * to large buffers (64K). Such pointers are normalized with a zero offset.
 * Since MSDOS is not a preemptive multitasking OS, this table is not
 * protected from concurrent access. This hack doesn't work anyway on
 * a protected system like OS/2. Use Microsoft C instead.
 */

zvoid far *zcalloc (unsigned items, unsigned size)
{
    zvoid far *buf;
    ulg bsize = (ulg)items*size;

    if (bsize < (65536L-16L)) {
        buf = farmalloc(bsize);
        if (*(ush*)&buf != 0) return buf;
    } else {
        buf = farmalloc(bsize + 16L);
    }
    if (buf == NULL || next_ptr >= MAX_PTR) return NULL;
    table[next_ptr].org_ptr = buf;

    /* Normalize the pointer to seg:0 */
    *((ush*)&buf+1) += ((ush)((uch*)buf-NULL) + 15) >> 4;
    *(ush*)&buf = 0;
    table[next_ptr++].new_ptr = buf;
    return buf;
}

zvoid zcfree (zvoid far *ptr)
{
    int n;
    if (*(ush*)&ptr != 0) { /* object < 64K */
        farfree(ptr);
        return;
    }
    /* Find the original pointer */
    for (n = next_ptr - 1; n >= 0; n--) {
        if (ptr != table[n].new_ptr) continue;

        farfree(table[n].org_ptr);
        while (++n < next_ptr) {
            table[n-1] = table[n];
        }
        next_ptr--;
        return;
    }
    ziperr(ZE_MEM, "zcfree: ptr not found");
}
#endif /* __TURBOC__ */

#if defined(MSC) || defined(__WATCOMC__)
#if (!defined(_MSC_VER) || (_MSC_VER < 700))
#  define _halloc  halloc
#  define _hfree   hfree
#endif

zvoid far *zcalloc (unsigned items, unsigned size)
{
    return (zvoid far *)_halloc((long)items, size);
}

zvoid zcfree (zvoid far *ptr)
{
    _hfree((void huge *)ptr);
}
#endif /* MSC || __WATCOMC__ */

#endif /* MY_ZCALLOC */

#if (defined(__WATCOMC__) && defined(ASMV) && !defined(__386__))
/* This is a hack to connect "call _exit" in match.asm to exit() */
#pragma aux xit "_exit" parm caller []
void xit(void)
{
    exit(20);
}
#endif

local int is_running_on_windows(void)
{
    char * var = getenv("OS");

    /* if the OS env.var says 'Windows_NT' then */
    /* we're likely running on a variant of WinNT */

    if ((NULL != var) && (0 == strcmp("Windows_NT", var)))
    {
        return 1;
    }

    /* if the windir env.var is non-null then */
    /* we're likely running on a variant of Win9x */
    /* DOS mode of Win9x doesn't define windir, only winbootdir */
    /* NT's command.com can't see lowercase env. vars */

    var = getenv("windir");
    if ((NULL != var) && (0 != var[0]))
    {
        return 1;
    }

    return 0;
}

void check_for_windows(char *app)
{
    /* Print a warning for users running under Windows */
    /* to reduce bug reports due to running DOS version */
    /* under Windows, when Windows version usually works correctly */

    /* This is only called from the DOS version */

    if (is_running_on_windows())
    {
        printf("\nzip warning:  You are running MSDOS %s on Windows.\n"
               "Try the Windows version before reporting any problems.\n",
               app);
    }
}

#endif /* !UTIL */


#ifndef WINDLL
/******************************/
/*  Function version_local()  */
/******************************/

static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s.\n\n";
                        /* At module level to keep Turbo C++ 1.0 happy !! */

void version_local()
{
#if defined(__DJGPP__) || defined(__WATCOMC__) || \
    (defined(_MSC_VER) && (_MSC_VER != 800))
    char buf[80];
#endif

/* Define the compiler name and version strings */
#if defined(__GNUC__)
#  if defined(__DJGPP__)
    sprintf(buf, "djgpp v%d.%02d / gcc ", __DJGPP__, __DJGPP_MINOR__);
#    define COMPILER_NAME1      buf
#  elif defined(__GO32__)         /* __GO32__ is defined as "1" only (sigh) */
#    define COMPILER_NAME1      "djgpp v1.x / gcc "
#  elif defined(__EMX__)          /* ...so is __EMX__ (double sigh) */
#    define COMPILER_NAME1      "emx+gcc "
#  else
#    define COMPILER_NAME1      "gcc "
#  endif
#  define COMPILER_NAME2        __VERSION__
#elif defined(__WATCOMC__)
#  if (__WATCOMC__ % 10 > 0)
/* We do this silly test because __WATCOMC__ gives two digits for the  */
/* minor version, but Watcom packaging prefers to show only one digit. */
    sprintf(buf, "Watcom C/C++ %d.%02d", __WATCOMC__ / 100,
            __WATCOMC__ % 100);
#  else
    sprintf(buf, "Watcom C/C++ %d.%d", __WATCOMC__ / 100,
            (__WATCOMC__ % 100) / 10);
#  endif
#  define COMPILER_NAME1        buf
#  define COMPILER_NAME2        ""
#elif defined(__TURBOC__)
#  ifdef __BORLANDC__
#    define COMPILER_NAME1      "Borland C++"
#    if (__BORLANDC__ < 0x0200)
#      define COMPILER_NAME2    " 1.0"
#    elif (__BORLANDC__ == 0x0200)   /* James:  __TURBOC__ = 0x0297 */
#      define COMPILER_NAME2    " 2.0"
#    elif (__BORLANDC__ == 0x0400)
#      define COMPILER_NAME2    " 3.0"
#    elif (__BORLANDC__ == 0x0410)   /* __BCPLUSPLUS__ = 0x0310 */
#      define COMPILER_NAME2    " 3.1"
#    elif (__BORLANDC__ == 0x0452)   /* __BCPLUSPLUS__ = 0x0320 */
#      define COMPILER_NAME2    " 4.0 or 4.02"
#    elif (__BORLANDC__ == 0x0460)   /* __BCPLUSPLUS__ = 0x0340 */
#      define COMPILER_NAME2    " 4.5"
#    elif (__BORLANDC__ == 0x0500)   /* __TURBOC__ = 0x0500 */
#      define COMPILER_NAME2    " 5.0"
#    else
#      define COMPILER_NAME2    " later than 5.0"
#    endif
#  else
#    define COMPILER_NAME1      "Turbo C"
#    if (__TURBOC__ > 0x0401)
#      define COMPILER_NAME2    "++ later than 3.0"
#    elif (__TURBOC__ == 0x0401)     /* Kevin:  3.0 -> 0x0401 */
#      define COMPILER_NAME2    "++ 3.0"
#    elif (__TURBOC__ == 0x0296)     /* [662] checked by SPC */
#      define COMPILER_NAME2    "++ 1.01"
#    elif (__TURBOC__ == 0x0295)     /* [661] vfy'd by Kevin */
#      define COMPILER_NAME2    "++ 1.0"
#    elif (__TURBOC__ == 0x0201)     /* Brian:  2.01 -> 0x0201 */
#      define COMPILER_NAME2    " 2.01"
#    elif ((__TURBOC__ >= 0x018d) && (__TURBOC__ <= 0x0200)) /* James: 0x0200 */
#      define COMPILER_NAME2    " 2.0"
#    elif (__TURBOC__ > 0x0100)
#      define COMPILER_NAME2    " 1.5"          /* James:  0x0105? */
#    else
#      define COMPILER_NAME2    " 1.0"          /* James:  0x0100 */
#    endif
#  endif
#elif defined(MSC)
#  if defined(_QC) && !defined(_MSC_VER)
#    define COMPILER_NAME1      "Microsoft Quick C"
#    define COMPILER_NAME2      ""      /* _QC is defined as 1 */
#  else
#    define COMPILER_NAME1      "Microsoft C "
#    ifdef _MSC_VER
#      if (_MSC_VER == 800)
#        define COMPILER_NAME2  "8.0/8.0c (Visual C++ 1.0/1.5)"
#      else
#        define COMPILER_NAME2 \
           (sprintf(buf, "%d.%02d", _MSC_VER/100, _MSC_VER%100), buf)
#      endif
#    else
#      define COMPILER_NAME2    "5.1 or earlier"
#    endif
#  endif
#else
#    define COMPILER_NAME1      "unknown compiler"
#    define COMPILER_NAME2      ""
#endif

/* Define the OS name and memory environment strings */
#if defined(__WATCOMC__) || defined(__TURBOC__) || defined(MSC) || \
    defined(__GNUC__)
#  define OS_NAME1      "\nMS-DOS"
#else
#  define OS_NAME1      "MS-DOS"
#endif

#if (defined(__GNUC__) || (defined(__WATCOMC__) && defined(__386__)))
#  define OS_NAME2      " (32-bit)"
#elif defined(M_I86HM) || defined(__HUGE__)
#  define OS_NAME2      " (16-bit, huge)"
#elif defined(M_I86LM) || defined(__LARGE__)
#  define OS_NAME2      " (16-bit, large)"
#elif defined(M_I86MM) || defined(__MEDIUM__)
#  define OS_NAME2      " (16-bit, medium)"
#elif defined(M_I86CM) || defined(__COMPACT__)
#  define OS_NAME2      " (16-bit, compact)"
#elif defined(M_I86SM) || defined(__SMALL__)
#  define OS_NAME2      " (16-bit, small)"
#elif defined(M_I86TM) || defined(__TINY__)
#  define OS_NAME2      " (16-bit, tiny)"
#else
#  define OS_NAME2      " (16-bit)"
#endif

/* Define the compile date string */
#ifdef __DATE__
#  define COMPILE_DATE " on " __DATE__
#else
#  define COMPILE_DATE ""
#endif

    printf(CompiledWith, COMPILER_NAME1, COMPILER_NAME2,
           OS_NAME1, OS_NAME2, COMPILE_DATE);

} /* end function version_local() */
#endif /* !WINDLL */


#if 0 /* inserted here for future use (clearing of archive bits) */
#if (defined(__GO32__) && (!defined(__DJGPP__) || (__DJGPP__ < 2)))

#include <errno.h>
int volatile _doserrno;

unsigned _dos_setfileattr(char *name, unsigned attr)
{
#if 0   /* stripping of trailing '/' is not needed for zip-internal use */
    unsigned namlen = strlen(name);
    char *i_name = alloca(namlen + 1);

    strcpy(i_name, name);
    if (namlen > 1 && i_name[namlen-1] == '/' && i_name[namlen-2] != ':')
        i_name[namlen-1] = '\0';
    asm("movl %0, %%edx": : "g" (i_name));
#else
    asm("movl %0, %%edx": : "g" (name));
#endif
    asm("movl %0, %%ecx": : "g" (attr));
    asm("movl $0x4301, %eax");
    asm("int $0x21": : : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi");
    _doserrno = 0;
    asm("jnc 1f");
    asm("movl %%eax, %0": "=m" (_doserrno));
    switch (_doserrno) {
    case 2:
    case 3:
           errno = ENOENT;
           break;
    case 5:
           errno = EACCES;
           break;
    }
    asm("1:");
    return (unsigned)_doserrno;
}

#endif /* DJGPP v1.x */
#endif /* never (not yet used) */


#if (defined(__DJGPP__) && (__DJGPP__ >= 2))

/* Disable determination of "x" bit in st_mode field for [f]stat() calls. */
int _is_executable (const char *path, int fhandle, const char *ext)
{
    return 0;
}

/* Prevent globbing of filenames.  This gives the same functionality as
 * "stubedit <program> globbing=no" did with DJGPP v1.
 */
#ifndef USE_DJGPP_GLOB
char **__crt0_glob_function(char *_arg)
{
    return NULL;
}
#endif

/* Reduce the size of the executable and remove the functionality to read
 * the program's environment from whatever $DJGPP points to.
 */
#if !defined(USE_DJGPP_ENV) || defined(UTIL)
void __crt0_load_environment_file(char *_app_name)
{
}
#endif

#endif /* __DJGPP__ >= 2 */


#if defined(_MSC_VER) && _MSC_VER == 700

/*
 * ARGH.  MSC 7.0 libraries think times are based on 1899 Dec 31 00:00, not
 *  1970 Jan 1 00:00.  So we have to diddle time_t's appropriately:  add
 *  70 years' worth of seconds for localtime() wrapper function;
 *  (70*365 regular days + 17 leap days + 1 1899 day) * 86400 ==
 *  (25550 + 17 + 1) * 86400 == 2209075200 seconds.
 *  Let time() and stat() return seconds since 1970 by using our own
 *  _dtoxtime() which is the routine that is called by these two functions.
 */


#ifdef UTIL
#  include <time.h>
#endif

#ifndef UTIL
#undef localtime
struct tm *localtime(const time_t *);

struct tm *msc7_localtime(const time_t *clock)
{
   time_t t = *clock;

   t += 2209075200L;
   return localtime(&t);
}
#endif /* !UTIL */


void __tzset(void);
int _isindst(struct tm *);

extern int _days[];

/* Nonzero if `y' is a leap year, else zero. */
#define leap(y) (((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)

/* Number of leap years from 1970 to `y' (not including `y' itself). */
#define nleap(y) (((y) - 1969) / 4 - ((y) - 1901) / 100 + ((y) - 1601) / 400)

time_t _dtoxtime(year, month, mday, hour, min, sec)
int year, month, mday, year, hour, min, sec;
{
   struct tm tm;
   time_t t;
   int days;

   days = _days[month - 1] + mday;
   year += 1980;
   if (leap(year) && month > 2)
     ++days;
   tm.tm_yday = days;
   tm.tm_mon = month - 1;
   tm.tm_year = year - 1900;
   tm.tm_hour = hour;
   __tzset();
   days += 365 * (year - 1970) + nleap (year);
   t = 86400L * days + 3600L * hour + 60 * min + sec + _timezone;
   if (_daylight && _isindst(&tm))
      t -= 3600;
   return t;
}

#endif /* _MSC_VER && _MSC_VER == 700 */


#ifdef __WATCOMC__

/* This papers over a bug in Watcom 10.6's standard library... sigh */
/* Apparently it applies to both the DOS and Win32 stat()s.         */

int stat_bandaid(const char *path, struct stat *buf)
{
  char newname[4];
  if (!stat(path, buf))
    return 0;
  else if (!strcmp(path, ".") || (path[0] && !strcmp(path + 1, ":."))) {
    strcpy(newname, path);
    newname[strlen(path) - 1] = '\\';   /* stat(".") fails for root! */
    return stat(newname, buf);
  } else
    return -1;
}

#endif
