/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include "zip.h"

#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

#include <time.h>

#define PATH_START '<'
#define PATH_END   '>'

/* Extra malloc() space in names for cutpath() */
#define PAD 5         /* may have to change .FOO] to ]FOO.DIR;1 */


#define TRUE 1
#define FALSE 0
#define O_RDONLY (0)
#define O_T20_WILD (1<<18)
#include <monsym.h>    /* Get amazing monsym() macro */
extern int jsys(), fstat();
extern char *getcwd();
extern int _gtjfn(), _rljfn();
#define JSYS_CLASS    0070000000000
#define FLD(val,mask) (((unsigned)(val)*((mask)&(-(mask))))&(mask))
#define _DEFJS(name,class) (FLD(class, JSYS_CLASS) | (monsym(name)&0777777))
#define JFNS  _DEFJS("JFNS%", 1)
#define GNJFN _DEFJS("GNJFN%", 0)
static int wfopen(), wfnext(), strlower(), strupper();
static char *wfname();
typedef struct {
   int  wfjfn;
   int  more;
} DIR;

/* Library functions not in (most) header files */

extern int stat(), chmod(), toupper(), tolower();

int utime OF((char *, ztimbuf *));

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Local functions */
local char *readd OF((DIR *));


local DIR *opendir(n)
char *n;                /* directory name to open */
/* Open the directory *n, returning a pointer to an allocated DIR, or
   NULL if error. */
{
    DIR *d;             /* pointer to malloc'ed directory stream */
    char    *c;                         /* scans TOPS20 path */
    int     m;                          /* length of name */
    char    *p;                         /* temp string */

    if (((d = (DIR *)malloc(sizeof(DIR))) == NULL) ||
        ((p = (char *)malloc((m = strlen(n)) + 4)) == NULL)) {
            return NULL;
    }

/* Directory may be in form "<DIR.SUB1.SUB2>" or "<DIR.SUB1>SUB2.DIRECTORY".
** If latter, convert to former. */

    if ((m > 0)  &&  (*(c = strcpy(p,n) + m-1) != '>')) {
        c -= 10;
        *c-- = '\0';        /* terminate at "DIRECTORY.1" */
        *c = '>';           /* "." --> ">" */
        while ((c > p)  &&  (*--c != '>'));
        *c = '.';           /* ">" --> "." */
    }
    strcat(p, "*.*");
    if ((d->wfjfn = wfopen(p)) == 0) {
        free((zvoid *)d);
        free((zvoid *)p);
        return NULL;
    }
    free((zvoid *)p);
    d->more = TRUE;
    return (d);
}

local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  char    *p;
  if ((d->more == FALSE) || ((p = wfname(d->wfjfn)) == NULL)) {
      return NULL;
  }
  if (wfnext(d->wfjfn) == 0) {
      d->more = FALSE;
  }
  return p;
}


local void closedir(d)
DIR *d;                 /* directory stream to close */
/* Close the directory stream */
{
  free((zvoid *)d);
}

/* Wildcard filename routines */

/* WFOPEN - open wild card filename
**      Returns wild JFN for filespec, 0 if failure.
*/
static int
wfopen(name)
char *name;
{
    return (_gtjfn(name, (O_RDONLY | O_T20_WILD)));
}

/* WFNAME - Return filename for wild JFN
**      Returns pointer to dynamically allocated filename string
*/
static char *
wfname(jfn)
int jfn;
{
    char *fp, fname[200];
    int ablock[5];

    ablock[1] = (int) (fname - 1);
    ablock[2] = jfn & 0777777;  /* jfn, no flags */
    ablock[3] = 0111110000001;  /* DEV+DIR+NAME+TYPE+GEN, punctuate */
    if (!jsys(JFNS, ablock))
        return NULL;            /* something bad happened */
    if ((fp = (char *)malloc(strlen(fname) + 1)) == NULL) {
        return NULL;
    }
    strcpy(fp, fname);          /* copy the file name here */
    return fp;
}

/* WFNEXT - Make wild JFN point to next real file
**      Returns success or failure (not JFN)
*/
static int
wfnext(jfn)
int jfn;
{
    int ablock[5];

    ablock[1] = jfn;            /* save jfn and flags */
    return jsys(GNJFN, ablock);
}


static int
strupper(s)     /* Returns s in uppercase */
char *s;        /* String to be uppercased */
{
    char    *p;

    p = s;
    for (; *p; p++)
        *p = toupper (*p);
}

static int
strlower(s)     /* Returns s in lowercase. */
char *s;        /* String to be lowercased */
{
    char    *p;

    p = s;
    for (; *p; p++)
        *p = tolower (*p);
}

int procname(n, caseflag)
char *n;                /* name to process */
int caseflag;           /* true to force case-sensitive match */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  DIR *d;               /* directory stream from opendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (LSSTAT(n, &s))
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
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  } else {
    if (dirnames && (m = newname(n, 1, caseflag)) != ZE_OK) {
      return m;
    }
    /* recurse into directory */
    if (recurse && (d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if ((m = procname(e, caseflag)) != ZE_OK)     /* recurse on name */
        {
          closedir(d);
          return m;
        }
      }
      closedir(d);
    }
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

  int jfn;
  char *fp, fname[200];
  int ablock[5];

  jfn = _gtjfn(x, (O_RDONLY));
  ablock[1] = (int) (fname - 1);
  ablock[2] = jfn & 0777777;    /* jfn, no flags */
  ablock[3] = 0111100000001;    /* DEV+DIR+NAME+TYPE, punctuate */
  if (!jsys(JFNS, ablock)) {
      _rljfn(jfn);
      return NULL;              /* something bad happened */
  }
  _rljfn(jfn);
  if ((fp = (char *)malloc(strlen(fname) + 1)) == NULL) {
      return NULL;
  }
  strcpy(fp, fname);            /* copy the file name here */
  x = fp;

  dosflag = dosify; /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  t = x;
  if ((n = strrchr(t, ':')) != NULL)
    t = n + 1;
  if (*t == PATH_START && (n = strrchr(t, PATH_END)) != NULL)
    if (*(++t) == '.')
      /* path is relative to current directory, skip leading '.' */
      t++;

  /* Make changes, if any, to the copied name (leave original intact) */
  if (!pathput)
    t = last(t, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;
  strcpy(n, t);

  if ((t = strrchr(n, PATH_END)) != NULL)
  {
    *t = '/';
    while (--t > n)
      if (*t == '.')
        *t = '/';
  }

  /* Fix from Greg Roelofs: */
  /* Get current working directory and strip from n (t now = n) */
  {
    char cwd[256], *p, *q;
    int c;

    if (getcwd(cwd, 256) && ((p = strchr(cwd, PATH_START)) != NULL))
    {
      if (*(++p) == '.')
        p++;
      if ((q = strrchr(p, PATH_END)) != NULL)
      {
        *q = '/';
        while (--q > p)
          if (*q == '.')
            *q = '/';

        /* strip bogus path parts from n */
        if (strncmp(n, p, (c=strlen(p))) == 0)
        {
          q = n + c;
          while (*t++ = *q++)
            ;
        }
      }
    }
  }
  strlower(n);

  if (isdir)
  {
    if (strcmp((t=n+strlen(n)-6), ".dir;1"))
      error("directory not version 1");
    else
      strcpy(t, "/");
  }

  if ((t = strrchr(n, '.')) != NULL)
  {
    if ( t[1] == '\0')             /* "filename." -> "filename" */
      *t = '\0';
  }

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
  char *t;              /* scans name */

  if ((t = strrchr(n, '/')) == NULL)
  {
    if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
      return NULL;
    strcpy(x, n);
  }
  else
  {
    if ((x = malloc(strlen(n) + 3 + PAD)) == NULL)
      return NULL;
    x[0] = PATH_START;
    x[1] = '.';
    strcpy(x + 2, n);
    *(t = x + 2 + (t - n)) = PATH_END;
    while (--t > x)
      if (*t == '/')
        *t = '.';
  }
  strupper(x);

  return x;
}

void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  ztimbuf u;            /* argument for utime() */

  /* Convert DOS time to time_t format in u */
  u.actime = u.modtime = dos2unixtime(d);

  /* Set updated and accessed times of f */
  utime(f, &u);
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
  /* converted from FNAMX to malloc - 11/8/04 EG */
  char *name;
  int len = strlen(f);

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

  if (strcmp(f, "-") == 0) {
    if (fstat(fileno(stdin), &s) != 0) {
      free(name);
      error("fstat(stdin)");
    }
  } else if (LSSTAT(name, &s) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(name);
    return 0;
  }
  free(name);

  if (a != NULL) {
    *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWRITE);
    if ((s.st_mode & S_IFDIR) != 0) {
      *a |= MSDOS_DIR_ATTR;
    }
  }
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;
  }

  return unix2dostime(&s.st_mtime);
}

#include <monsym.h>   /* Get amazing monsym() macro */
#define       _FBBYV  monsym(".FBBYV")
#define         FBBSZ_S       -24     /* Obsolete, replace by FLDGET! */
#define         FBBSZ_M       077     /* ditto */

extern int _gtjfn(), _rljfn(), _gtfdb(), stat();

int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
  /* create extra field and change z->att if desired */
{
  int jfn;

  translate_eol = 0;
  jfn = _gtjfn(z->name, O_RDONLY);
  z->att = (((_gtfdb (jfn, _FBBYV) << FBBSZ_S) & FBBSZ_M) != 8) ?
           ASCII :BINARY;
  _rljfn(jfn);

#ifdef USE_EF_UT_TIME
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
#endif /* USE_EF_UT_TIME */

  return ZE_OK;
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
   For VMS, d must be in format [x.y]z.dir;1  (not [x.y.z]).
 */
{
    zipwarn("deletedir not implemented yet", "");
    return 127;
}

#endif /* !UTIL */


/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
#if 0
    char buf[40];
#endif

    printf(CompiledWith,

#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
#  if 0
      "cc ", (sprintf(buf, " version %d", _RELEASE), buf),
#  else
#  ifdef __COMPILER_KCC__
      "KCC", "",
#  else
      "unknown compiler", "",
#  endif
#  endif
#endif

      "TOPS-20",

#if defined(foobar) || defined(FOOBAR)
      " (Foo BAR)",   /* OS version or hardware */
#else
      "",
#endif /* Foo BAR */

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

} /* end function version_local() */
