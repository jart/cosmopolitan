/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  theos.c (zip)

  Contribution by Jean-Michel Dubois. 20-Jun-1995, 20-Dec-98

  THEOS specific extra informations

  ---------------------------------------------------------------------------*/


#include <stdio.h>

#ifndef UTIL

#include "zip.h"

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sc.h>
#include <direct.h>
#include <sys/utime.h>

#define opendir(a) _opendir(a)
extern struct dirent* _opendir(char* fname);

#define PAD 0

#define RET_ERROR 1
#define RET_SUCCESS 0
#define RET_EOF 0

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* match from Phase One Systems */

int match(char *s, char *p)    /*S Returns non-zero if string matches
                                   the literal mask */
{
  int matched, k;

  if (!(*p))
    return 1;
  for(;;) {
    if ( (!(*s)) && (!(*p)) )
      return(1);
    else if ( !(*p) )
      return(0);
    else if (*p == '*') {
      if (!*(p+1))
        return(1);
      k=0;
      do {
        matched = match(s+k,p+1);
        k++;
      } while ( (!matched) && *(s+k));
      return(matched);
    } else if (*p == '@') {
      if (!((*s >= 'a' && *s <= 'z')
          ||(*s >= 'A' && *s <= 'Z')))
        return(0);
    } else if (*p == '#') {
      if (*s < '0' || *s > '9')
        return(0);
    } else if (*p != '?') {
      if (tolower(*s) != tolower(*p))
        return(0);
    }
    s++; p++;
  }
}

local char *readd(d)
DIR *d; /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

/* check if file is a member of a library */

int ismember(char* path)
{
    char* p;

    if ((p = strrchr(path, '/')) == NULL)
        p = path;
    return ((p = strchr(p, '.')) && (p = strchr(p + 1, '.')));
}

/* extract library name from a file name */

char* libname(char* path)
{
    char* p;
    static char lib[FILENAME_MAX];
    char drive[3];

    strcpy(lib, path);
    if (p = strrchr(lib, ':')) {
        strncpy(drive, p, 2);
        drive[2] = '\0';
        *p = '\0' ;
    } else
        drive[0] = '\0';

    if ((p = strrchr(lib, '/')) == NULL)
        p = lib;

    p = strchr(p, '.');
    p = strchr(p + 1, '.');
    *p = 0;
    strcat(lib, drive);
    return lib;
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
  struct zlist *z;      /* steps through zfiles list */
  struct flist *f;      /* steps through files list */
  char* path;           /* full name */
  char drive[3];        /* drive name */
  int recursion;        /* save recurse flag */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (LSSTAT(n, &s)) {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (match(z->iname, p))
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

  /* Live name--use if file, recurse if directory or library */
  if (S_ISREG(s.st_mode)) {
    if ((path = malloc(strlen(n) + 2)) == NULL)
      return ZE_MEM;

    strcpy(path, n);

    /* if member name, include library name before any member name */
    if (ismember(path)) {
      strcpy(path, libname(path));
      /* mask recursion flag to avoid endless loop recursion
       * if -r is used with member names
       */
      recursion = recurse;
      recurse = FALSE;
      if ((m = procname(path, caseflag)) != ZE_OK)   /* recurse on name */
      {
        if (m == ZE_MISS)
          zipwarn("name not matched: ", path);
        else
          ziperr(m, a);
      }
      /* restore recursion flag */
      recurse = recursion;
    }

    strcpy(path, n);

    if ((p = strchr(path, ':')) != NULL) {
      p[2] = '\0';
      strcpy(drive, p);
    } else
      drive[0] = '\0';

    /* remove trailing dot in flat file name */
    p = strend(path) - 1;
    if (*p == '.')
      *p = '\0';

    strcat(path, drive);
    /* add or remove name of file */
    if ((m = newname(path, 0, caseflag)) != ZE_OK) {
      free(path);
      return m;
    }
    free(path);
  } else if (S_ISLIB(s.st_mode)) {
    if ((path = malloc(strlen(n) + 2)) == NULL)
      return ZE_MEM;

    strcpy(path, n);

    if ((p = strchr(path, ':')) != NULL) {
      p[2] = '\0';
      strcpy(drive, p);
    } else
      drive[0] = '\0';

    /* add a trailing dot in flat file name... */
    p = strend(path) - 1;
    if (*p != '/')
      strcat(path, "/");
    p = strend(path);
    /* ... then add drive name */
    strcpy(p, drive);

    /* don't include the library name twice... or more */
    for (f = found; f != NULL; f = f->nxt) {
      if (! stricmp(path, f->name)) {
        free(path);
        return ZE_OK;
      }
    }
    /* add or remove name of library */
    if ((m = newname(path, 0, caseflag)) != ZE_OK) {
      free(path);
      return m;
    }
    /* recurse into library if required */
    strcpy(p - 1, ".*");
    strcat(p, drive);
    if (recurse && diropen(path) == 0)
    {
      while ((e = dirread()) != NULL) {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if (*drive == '\0')
            *strchr(e, ':') = '\0';
          if ((a = malloc(strlen(e) + 1)) == NULL)
          {
            dirclose();
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcpy(a, e);
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
      dirclose();
    }
    free(path);
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
  }
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
  char *p;
  int dosflag;

  dosflag = dosify;  /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  for (t = x; *t == '/'; t++)
    ;

  /* Make changes, if any, to the copied name (leave original intact) */
  if (!pathput)
    t = last(t, '/');

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;

  strcpy(n, t);
  if (p = strchr(n, ':'))
    *p = '\0';
  for (p = n; *p = tolower(*p); p++);

  if (isdir == 42) return n;      /* avoid warning on unused variable */

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

/*
 * XXX use ztimbuf in both POSIX and non POSIX cases ?
 */
void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  struct utimbuf u;     /* argument for utime()  const ?? */

  /* Convert DOS time to time_t format in u */
  u.actime = u.modtime = dos2unixtime(d);
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
   of that to DOS attributes. Bits 8 to 15 contains native THEOS protection
   code. If n is not NULL, store the file size there.  If t is not NULL,
   the file's access, modification and creation times are stored there as
   UNIX time_t values. If f is "-", use standard input as the file. If f is
   a device, return a file size of -1 */
{
  struct stat s;        /* results of stat() */
  /* from FNMAX to malloc - 11/8/04 EG */
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

  if (name[len - 1] == '/' || name[len - 1] == '.')
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
    if ((s.st_mode & S_IFMT) == S_IFDIR
     || (s.st_mode & S_IFMT) == S_IFLIB) {
      *a |= MSDOS_DIR_ATTR;
    }
    /* Map Theos' hidden attribute to DOS's hidden attribute */
    if (!(st.st_protect & 0x80))
      *a |= MSDOS_HIDDEN_ATTR;
    *a |= ((ulg) s.st_protect) << 8;
  }
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = t->mtime;   /* best guess, (s.st_ctime: last status change!!) */
  }
  return unix2dostime(&s.st_mtime);
}
/*
 *      Get file THEOS attributes and store them into extent fields.
 *      On error leave z intact.
 */

/*
*   Extra record format
*   ===================
*   signature       (2 bytes)   = 'T','h'
*   size            (2 bytes)
*   flags           (1 byte)
*   filesize        (4 bytes)
*   keylen          (2 bytes)
*   reclen          (2 bytes)
*   filegrow        (1 byte)
*   reserved        (4 bytes)
*/

#define EB_L_THSIZE     4
#define EB_L_TH_SIZE    14

int set_extra_field(z, z_utim)
  struct zlist *z;
  iztimes *z_utim;
  /* store full data in local header but just modification time stamp info
     in central header */
{
  char *extra = NULL;
  char *p;
  char c;
  struct stat st;
  int status;

  if (status = stat(z->name, &st)) {
    p = &z->name[strlen(z->name) - 1];
    if (*p == '/' || *p == '.') {
      c = *p;
      *p = '\0';
      status = stat(z->name, &st);
      *p = c;
    }
#ifdef DEBUG
    fprintf(stderr, "set_extra_field: stat for file %s:\n status = %d\n",
        z->name, status);
#endif
    if (status)
      return RET_ERROR;
  }

  if ((extra = malloc(EB_L_TH_SIZE)) == NULL ) {
    fprintf(stderr, "set_extra_field: Insufficient memory.\n" );
    return RET_ERROR;
  }


  extra[0] = 'T';
  extra[1] = 'h';
  extra[2] = EB_L_TH_SIZE;
  extra[3] = EB_L_TH_SIZE >> 8;
  extra[4] = 0;
  extra[5] = st.st_size;
  extra[6] = st.st_size >> 8;
  extra[7] = st.st_size >> 16;
  extra[8] = st.st_size >> 24;
  extra[9] = st.st_org;
  extra[10] = st.st_rlen;
  extra[11] = st.st_rlen >> 8;
  extra[12] = st.st_klen;
  extra[13] = st.st_klen >> 8;
  extra[14] = st.st_grow;
  extra[15] = st.st_protect;
  extra[16] = 0;
  extra[17] = 0;
  z->ext = z->cext = EB_L_TH_SIZE + EB_HEADSIZE;
  z->extra = z->cextra = extra;
  return RET_SUCCESS;
}
#endif

/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
  printf("Compiled with THEOS C 5.28 for THEOS 4.x on %s %s.\n\n",
    __DATE__, __TIME__);
}

