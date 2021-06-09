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
#include <errno.h>
#include <dirent.h>
#include <mintbind.h>
#include <osbind.h>
#include <ostruct.h>


#define PAD 0
#define PATH_END '/'

extern char *label;     /* defined in fileio.c */

local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;


local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

local char *getVolumeLabel(drive, vtime, vmode, utim)
  int drive;  /* drive name: 'A' .. 'Z' or '\0' for current drive */
  ulg *vtime; /* volume label creation time (DOS format) */
  ulg *vmode; /* volume label file mode */
  time_t utim;/* volume label creation time (UNIX format) */

/* If a volume label exists for the given drive, return its name and
   set its time and mode. The returned name must be static data. */
{
  static char vol[14];
  _DTA *dtaptr;

  if (drive) {
    vol[0] = (char)drive;
    strcpy(vol+1, ":/");
  } else {
    strcpy(vol, "/");
  }
  strcat(vol, "*.*");
  if (Fsfirst(vol, FA_LABEL) == 0) {
    dtaptr = Fgetdta();
    strncpy(vol, dtaptr->dta_name, sizeof(vol)-1);
    *vtime = ((ulg)dtaptr->dta_date << 16) |
             ((ulg)dtaptr->dta_time & 0xffff);
    *vmode = (ulg)dtaptr->dta_attribute;
    return vol;
  }
  return NULL;
}

char GetFileMode(char *name)
{
   struct stat sb;

   sb.st_attr = 0;
   Fxattr(linkput ? 1 : 0, name, &sb);
   if (errno == EINVAL) {
      _DTA *dtaptr, *old;
      old = Fgetdta();
      Fsfirst(name, FA_RDONLY+FA_HIDDEN+FA_SYSTEM+FA_DIR);
      dtaptr = Fgetdta();
      sb.st_attr = dtaptr->dta_attribute;
      Fsetdta(old);
   }
   return sb.st_attr & 0x3f;
}


int wild2(w)
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
    label = getVolumeLabel(w[1] == ':' ? to_up(w[0]) : '\0',
                           &label_time, &label_mode, &label_utim);
    if (label != NULL) {
       newname(label, 0, 0);
    }
    if (w[1] == ':' && w[2] == '\0') return ZE_OK;
    /* "zip -$ foo a:" can be used to force drive name */
  }

  /* special handling of stdin request */
  if (strcmp(w, "-") == 0)   /* if compressing stdin */
    return newname(w, 0, 0);

  /* Allocate and copy pattern */
  if ((p = a = malloc(strlen(w) + 1)) == NULL)
    return ZE_MEM;
  strcpy(p, w);

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
    q = "*.*";
  }
  else                                  /* no path or device */
  {
    q = p;
    p = strcpy(v, ".");
  }
  if (recurse && *q == '\0') {
    q = "*.*";
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


#include <regexp.h>
#include <osbind.h>

void regerror( char ZCONST *msg ) {
   perror( msg );
}

static int    ret;
static regexp *regptr;
static short  is_w, ind_w;
static char   fullpath[FILENAME_MAX], file_arg[FILENAME_MAX];

#define FTW_F   0
#define FTW_D   1
#define FTW_DNR 2
#define FTW_NS  3

static int ftwfunc( struct stat *stats, int ftw_status )
{
   char *path = &fullpath[0];

   if (strncmp(path, "./", 2) == 0) path += 2;
   switch (ftw_status) {
   case FTW_NS:
      zipwarn("can't stat file: ", path);
      ret = ZE_MISS;
      return 0;
   case FTW_F:
      if (!is_w || regexec(regptr, path, 1)) {
#if 0
         char fn[FILENAME_MAX];
         int  k;
         if (S_ISLNK(stats->st_mode) &&
             (k = readlink(path, fn, FILENAME_MAX)) > 0) {
            int l = strlen(path);
            fn[k] = '\0';
            strcat(strcat(path, " -> "), fn);
            ret = newname(path, 0, 0); /* procname(path, 0); */
            path[l] = '\0';
         } else
#endif
            ret = newname(path, 0, 0); /* procname(path, 0); */
      }
      return 0;
   case FTW_DNR:
      zipwarn("can't open directory: ", path);
      ret = ZE_MISS;
      return 0;
   case FTW_D:
      if (strcmp(path, ".") == 0) return 0;
      if (is_w && ind_w > 0 && strncmp(path, file_arg, ind_w) != 0)
         return 4;
   }
   return 0;
}

static int myftw( int depth )
{
   register DIR   *dirp;
   struct dirent  *entp;
   struct stat    stats;
   register char  *p,*q;
   register long  i;

   if (LSSTAT(fullpath, &stats) < 0)
      return ftwfunc(&stats, FTW_NS);

   if (!S_ISDIR(stats.st_mode))
      return ftwfunc(&stats, FTW_F);

   if ((dirp = opendir(fullpath)) == NULL)
      return ftwfunc(&stats, FTW_DNR);

   if (i = ftwfunc(&stats, FTW_D)) {
      closedir(dirp);
      return (i == 4 ? 0 : (int)i);
   }
   i = strlen(fullpath);
   p = &fullpath[i];
   *p++ = '/'; *p = '\0';
   if (dirnames && i > 1) {
      q = (strncmp(fullpath, "./", 2) == 0 ? &fullpath[2] : &fullpath[0]);
      ret = newname(q, 1, 0);
   }
   i = 0;
   while (depth > 0 && (entp = readdir(dirp)) != 0)
      if (strcmp(entp->d_name,".") != 0 && strcmp(entp->d_name,"..") != 0) {
         strcpy(p, entp->d_name);
         if (i = myftw(depth-1))
            depth = 0;       /* force User's finish */
      }
   closedir(dirp);
   return (int)i;
}

int wild( char *p )
{
   char *d;

   ret = ZE_OK;
   if (p == NULL) p = "*";

   if (strcmp(p, "-") == 0)     /* if compressing stdin */
      ret = newname(p, 0, 0);

   strcpy(fullpath, p);
   /* now turning UNIX-Wildcards into basic regular expressions */
   for (is_w = ind_w = 0, d = &file_arg[0]; *p; d++, p++)
      switch (*p) {
      case '*': *d++ = '.'; *d = *p; is_w = 1;  break;
      case '?': *d = '.';            is_w = 1;  break;
      case '[': *d = *p;
                if (*(p+1) == '!') {
                   *++d = '^'; p++;
                }                    is_w = 1;  break;
      case '.': *d++ = '\\'; *d = *p;           break;
      default : *d = *p;
                if (!is_w) ind_w++;
      }
   *++d = '\0';
   if (is_w) {
      strcat( file_arg, "$" );           /* to get things like *.[ch] work */
      if ((regptr = regcomp( file_arg )) == NULL)
         return ZE_MEM;
      strcpy( fullpath, "." );
      myftw( recurse ? 99 : 1 );
      free(regptr);
   } else if (recurse) {
      myftw( 99 );
   } else
      myftw( 1 ); /* ret = procname( fullpath, 0 ); */
   return ret;
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
  for (p = n; *p; p++)          /* use / consistently */
    if (*p == '\\')
      *p = '/';
  if (!S_ISDIR(s.st_mode))
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  } else {
    /* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    if (strcmp(n, ".") == 0) {
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
  char *t, *p;          /* shortened name */
  int dosflag;

  dosflag = 0;

  /* Find starting point in name before doing malloc */
  t = *x && *(x + 1) == ':' ? x + 2 : x;
  while (*t == '/' || *t == '\\')
    t++;

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
#if 0
  if (p = strstr(t, " -> "))       /* shorten "link -> data" to "link" */
    *p = '\0';
#endif
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
  struct utimbuf u;     /* argument for utime()  const ?? */

  /* Convert DOS time to time_t format in u[0] and u[1] */
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

  if (a != NULL) {
/*  *a = ((ulg)s.st_mode << 16) | (ulg)GetFileMode(name); */
    *a = ((ulg)s.st_mode << 16) | (ulg)s.st_attr;
  }
  free(name);
  if (n != NULL)
    *n = S_ISREG(s.st_mode) ? s.st_size : -1L;
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

  if ((z->extra = (char *)malloc(EB_HEADSIZE+EB_UT_LEN(2))) == NULL)
    return ZE_MEM;
  if ((z->cextra = (char *)malloc(EB_HEADSIZE+EB_UT_LEN(1))) == NULL)
    return ZE_MEM;

  z->extra[0]  = 'U';
  z->extra[1]  = 'T';
  z->extra[2]  = EB_UT_LEN(2);          /* length of data part of e.f. */
  z->extra[3]  = 0;
  z->extra[4]  = EB_UT_FL_MTIME | EB_UT_FL_ATIME;
  z->extra[5]  = (char)(z_utim->mtime);
  z->extra[6]  = (char)(z_utim->mtime >> 8);
  z->extra[7]  = (char)(z_utim->mtime >> 16);
  z->extra[8]  = (char)(z_utim->mtime >> 24);
  z->extra[9]  = (char)(z_utim->atime);
  z->extra[10] = (char)(z_utim->atime >> 8);
  z->extra[11] = (char)(z_utim->atime >> 16);
  z->extra[12] = (char)(z_utim->atime >> 24);

  z->ext = (EB_HEADSIZE+EB_UX_LEN(2));

  memcpy(z->cextra, z->extra, (EB_HEADSIZE+EB_UT_LEN(1)));
  z->cextra[EB_LEN] = EB_UT_LEN(1);
  z->cext = (EB_HEADSIZE+EB_UX_LEN(1));

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

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
#ifdef __TURBOC__
    char buf[40];
#endif

    printf(CompiledWith,

#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
#  if 0
      "cc ", (sprintf(buf, " version %d", _RELEASE), buf),
#  else
#  ifdef __TURBOC__
      "Turbo C", (sprintf(buf, " (0x%04x = %d)", __TURBOC__, __TURBOC__), buf),
#  else
      "unknown compiler", "",
#  endif
#  endif
#endif

#ifdef __MINT__
      "Atari TOS/MiNT",
#else
      "Atari TOS",
#endif

      " (Atari ST/TT/Falcon030)",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

} /* end function version_local() */
