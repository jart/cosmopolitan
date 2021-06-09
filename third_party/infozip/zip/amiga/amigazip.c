/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include "zip.h"
#include "amiga/amiga.h"

#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

#define utime FileDate

#define PAD 0
#define PATH_END '/'

/* Local globals (kinda like "military intelligence" or "broadcast quality") */

extern char *label;             /* still declared in fileio.c */
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Local functions */
local char *readd OF((DIR *));
local int wild_recurse OF((char *, char *));


local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}


/* What we have here is a mostly-generic routine using opendir()/readd() and */
/* isshexp()/MATCH() to find all the files matching a multi-part filespec  */
/* using the portable pattern syntax.  It shouldn't take too much fiddling */
/* to make it usable for any other platform that has directory hierarchies */
/* but no shell-level pattern matching.  It works for patterns throughout  */
/* the pathname, such as "foo:*.?/source/x*.[ch]".                         */

#define ONENAMELEN 30
/* the length of one filename component on the Amiga */

/* whole is a pathname with wildcards, wildtail points somewhere in the  */
/* middle of it.  All wildcards to be expanded must come AFTER wildtail. */

local int wild_recurse(whole, wildtail) char *whole; char *wildtail;
{
    DIR *dir;
    char *subwild, *name, *newwhole = NULL, *glue = NULL, plug = 0, plug2;
    ush newlen, amatch = 0;
    BPTR lok;
    int e = ZE_MISS;

    if (!isshexp(wildtail))
        if (lok = Lock(whole, ACCESS_READ)) {       /* p exists? */
            UnLock(lok);
            return procname(whole, 0);
        } else
            return ZE_MISS;                     /* woops, no wildcards! */

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
        dir = opendir(whole);
    } while (!dir && !disk_not_mounted && subwild > wildtail);
    wildtail = subwild;                 /* skip past non-wild components */

    if ((subwild = strchr(wildtail + 1, PATH_END)) != NULL) {
        /* this "+ 1" dodges the   ^^^ hole left by *glue == 0 */
        *(subwild++) = 0;               /* wildtail = one component pattern */
        newlen = strlen(whole) + strlen(subwild) + (ONENAMELEN + 2);
    } else
        newlen = strlen(whole) + (ONENAMELEN + 1);
    if (!dir || !(newwhole = malloc(newlen))) {
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

    while (name = readd(dir)) {
        if (MATCH(wildtail, name, 0)) {
            strcpy(newwhole + newlen, name);
            if (subwild) {
                name = newwhole + strlen(newwhole);
                *(name++) = PATH_END;
                strcpy(name, subwild);
                e = wild_recurse(newwhole, name);
            } else
                e = procname(newwhole, 0);
            newwhole[newlen] = 0;
            if (e == ZE_OK)
                amatch = 1;
            else if (e != ZE_MISS)
                break;
        }
    }

  ohforgetit:
    if (dir) closedir(dir);
    if (subwild) *--subwild = PATH_END;
    if (newwhole) free(newwhole);
    if (e == ZE_MISS && amatch)
        e = ZE_OK;
    return e;
}

int wild(p) char *p;
{
    char *use;

    /* special handling of stdin request */
    if (strcmp(p, "-") == 0)    /* if compressing stdin */
        return newname(p, 0, 0);

    /* wild_recurse() can't handle colons in wildcard part: */
    if (use = strchr(p, ':')) {
        if (strchr(++use, ':'))
            return ZE_PARMS;
    } else
        use = p;

    return wild_recurse(p, use);
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

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (LSSTAT(n, &s))
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
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  } else {
    /* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    strcpy(p, n);
    a = p + strlen(p);
    if (*p && a[-1] != '/' && a[-1] != ':')
      strcpy(a, "/");
    if (dirnames && (m = newname(p, 1, caseflag)) != ZE_OK) {
      free((zvoid *)p);
      return m;
    }
    /* recurse into directory */
    if (recurse && (d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
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

  dosflag = dosify;     /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  if ((t = strrchr(x, ':')) != NULL)    /* reject ":" */
    t++;
  else
    t = x;
  {                                     /* reject "//" */
    char *tt = t;
    while (tt = strchr(tt, '/'))
      while (*++tt == '/')
        t = tt;
  }
  while (*t == '/')             /* reject leading "/" on what's left */
    t++;

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
  time_t u[2];          /* argument for utime() */

  /* Convert DOS time to time_t format in u */
  u[0] = u[1] = dos2unixtime(d);

  /* Set updated and accessed times of f */
  utime(f, u);
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
    if (fstat(fileno(stdin), &s) != 0)
      error("fstat(stdin)");
  } else if (SSTAT(name, &s) != 0) {
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

  z->cextra = z->extra;
  z->cext = z->ext  = (EB_HEADSIZE+EB_UT_LEN(1));

  return ZE_OK;
#else /* !USE_EF_UT_TIME */
  return (int)(z-z);
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


/******************************/
/*  Function version_local()  */
/******************************/


/* NOTE:  the following include depends upon the environment
 *        variable $Workbench to be set correctly.  (Set by
 *        default, by Version command in Startup-sequence.)
 */
int WBversion = (int)
#include "ENV:Workbench"
;

void version_local()
{
   static ZCONST char CompiledWith[] = "Compiled with %s%s under %s%s%s%s.\n\n";

/* Define buffers. */

   char buf1[16];  /* compiler name */
   char buf2[16];  /* revstamp */
   char buf3[16];  /* OS */
   char buf4[16];  /* Date */
/*   char buf5[16];  /* Time */

/* format "with" name strings */

#ifdef AMIGA
# ifdef __SASC
   strcpy(buf1,"SAS/C ");
# else
#  ifdef LATTICE
    strcpy(buf1,"Lattice C ");
#  else
#   ifdef AZTEC_C
     strcpy(buf1,"Manx Aztec C ");
#   else
     strcpy(buf1,"UNKNOWN ");
#   endif
#  endif
# endif
/* "under" */
  sprintf(buf3,"AmigaDOS v%d",WBversion);
#else
  strcpy(buf1,"Unknown compiler ");
  strcpy(buf3,"Unknown OS");
#endif

/* Define revision, date, and time strings.
 * NOTE:  Do not calculate run time, be sure to use time compiled.
 * Pass these strings via your makefile if undefined.
 */

#if defined(__VERSION__) && defined(__REVISION__)
  sprintf(buf2,"version %d.%d",__VERSION__,__REVISION__);
#else
# ifdef __VERSION__
  sprintf(buf2,"version %d",__VERSION__);
# else
  sprintf(buf2,"unknown version");
# endif
#endif

#ifdef __DATE__
  sprintf(buf4," on %s",__DATE__);
#else
  strcpy(buf4," unknown date");
#endif

/******
#ifdef __TIME__
  sprintf(buf5," at %s",__TIME__);
#else
  strcpy(buf5," unknown time");
#endif
******/

/* Print strings using "CompiledWith" mask defined above.
 *  ("Compiled with %s%s under %s%s%s%s.")
 */

   printf(CompiledWith,
     buf1,
     buf2,
     buf3,
     buf4,
     /* buf5, */ "",
     "" );  /* buf6 not used */

} /* end function version_local() */
