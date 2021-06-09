/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#include <stdlib.h>
#include <string.h>
#include "zip.h"

#ifndef UTIL

#define PAD 0
#define PATH_END '/'


local int wild_recurse(char *whole, char *wildtail);
local int uxtime2acornftime(unsigned *pexadr, unsigned *pldadr, time_t ut);

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

char *readd(DIR *d)
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
   struct dirent *e;

   e = readdir(d);
   return (e == NULL ? (char *) NULL : e->d_name);
}

/* What we have here is a mostly-generic routine using opend()/readd() and */
/* isshexp()/MATCH() to find all the files matching a multi-part filespec  */
/* using the portable pattern syntax.  It shouldn't take too much fiddling */
/* to make it usable for any other platform that has directory hierarchies */
/* but no shell-level pattern matching.  It works for patterns throughout  */
/* the pathname, such as "foo:*.?/source/x*.[ch]".                         */

/* whole is a pathname with wildcards, wildtail points somewhere in the  */
/* middle of it.  All wildcards to be expanded must come AFTER wildtail. */

local int wild_recurse(whole, wildtail) char *whole; char *wildtail;
{
    DIR *dir;
    char *subwild, *name, *newwhole = NULL, *glue = NULL, plug = 0, plug2;
    ush newlen, amatch = 0;
    struct stat statb;
    int disk_not_mounted=0;
    int e = ZE_MISS;

    if (!isshexp(wildtail)) {
        if (stat(whole,&statb)==0 && (statb.st_mode & S_IREAD)!=0) {
            return procname(whole, 0);
        } else
            return ZE_MISS;                     /* woops, no wildcards! */
    }

    /* back up thru path components till existing dir found */
    do {
        name = wildtail + strlen(wildtail) - 1;
        for (;;)
            if (name-- <= wildtail || *name == '.') {
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

    if ((subwild = strchr(wildtail + 1, '.')) != NULL) {
        /* this "+ 1" dodges the   ^^^ hole left by *glue == 0 */
        *(subwild++) = 0;               /* wildtail = one component pattern */
        newlen = strlen(whole) + strlen(subwild) + 32;
    } else
        newlen = strlen(whole) + 31;
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
                *(name++) = '.';
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
    if (subwild) *--subwild = '.';
    if (newwhole) free(newwhole);
    if (e == ZE_MISS && amatch)
        e = ZE_OK;
    return e;
}

int wild(p)
char *p;
{
  char *path;
  int toret;

  /* special handling of stdin request */
  if (strcmp(p, "-") == 0)   /* if compressing stdin */
    return newname(p, 0, 0);

  path=p;
  if (strchr(p, ':')==NULL && *p!='@') {
    if (!(path=malloc(strlen(p)+3))) {
      return ZE_MEM;
    }
    strcpy(path,"@.");
    strcat(path,p);
  }

  toret=wild_recurse(path, path);

  if (path!=p) {
    free(path);
  }
  return toret;
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
    if (strcmp(n, ".") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (a[-1] != '.')
        strcpy(a, ".");
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
  char *tmp;
  int dosflag;
  char *lastlastdir=NULL;   /* pointer to 2 dirs before... */
  char *lastdir=NULL;       /* pointer to last dir... */

  /* Malloc space for internal name and copy it */
  if ((tmp = malloc(strlen(x) + 1)) == NULL)
    return NULL;
  strcpy(tmp, x);

  dosflag = dosify; /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  for(t=tmp;*t;t++) {
    if (*t=='/') {
      *t='.';
    }
    else if (*t=='.') {
      *t='/';
      lastlastdir=lastdir;
      lastdir=t+1;
    }
  }

  t=strchr(tmp,'$');      /* skip FS name */
  if (t!=NULL)
    t+=2;                 /* skip '.' after '$' */
  else
    t=tmp;
  if (*t=='@')            /* skip @. at the beginning of filenames */
    t+=2;

  /* Make changes, if any, to the copied name (leave original intact) */

  /* return a pointer to '\0' if the file is a directory with the same
     same name as an extension to swap (eg. 'c', 'h', etc.) */
  if (isdir && exts2swap!=NULL) {
    if (lastlastdir==NULL)
      lastlastdir=t;
    if (checkext(lastlastdir)) {
      free((void *)tmp);
      n=malloc(1);
      if (n!=NULL)
        *n='\0';
      return n;
    }
  }

  if (exts2swap!=NULL && lastdir!=NULL) {
    if (lastlastdir==NULL)
      lastlastdir=t;
    if (checkext(lastlastdir)) {
      if (swapext(lastlastdir,lastdir-1)) {
        free((void *)tmp);
        return NULL;
      }
    }
  }

  if (!pathput)
    t = last(t, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL) {
    free((void *)tmp);
    return NULL;
  }
  strcpy(n, t);

  free((void *)tmp);

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
  char *lastext=NULL;   /* pointer to last extension */
  char *lastdir=NULL;   /* pointer to last dir */

  if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
    return NULL;
  strcpy(x, n);

  for(t=x;*t;t++) {
    if (*t=='.') {
      *t='/';
      lastext=t+1;
    }
    else if (*t=='/') {
      *t='.';
      lastdir=t+1;
    }
  }

  if (exts2swap!=NULL && (int)lastext>(int)lastdir) {
    if (lastdir==NULL)
      lastdir=x;
    if (checkext(lastext)) {
      if (swapext(lastdir,lastext-1)) {
        free((void *)x);
        return NULL;
      }
    }
  }

  return x;
}

local int uxtime2acornftime(unsigned *pexadr, unsigned *pldadr, time_t ut)
{
  unsigned timlo;       /* 3 lower bytes of acorn file-time plus carry byte */
  unsigned timhi;       /* 2 high bytes of acorn file-time */

  timlo = ((unsigned)ut & 0x00ffffffU) * 100 + 0x00996a00U;
  timhi = ((unsigned)ut >> 24);
  timhi = timhi * 100 + 0x0000336eU + (timlo >> 24);
  if (timhi & 0xffff0000U)
      return 1;         /* calculation overflow, do not change time */

  /* insert the five time bytes into loadaddr and execaddr variables */
  *pexadr = (timlo & 0x00ffffffU) | ((timhi & 0x000000ffU) << 24);
  *pldadr = (*pldadr & 0xffffff00U) | ((timhi >> 8) & 0x000000ffU);

  return 0;             /* subject to future extension to signal overflow */
}

void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  time_t m_time;
  unsigned int loadaddr, execaddr;
  int attr;

  /* Convert DOS time to time_t format in m_time */
  m_time = dos2unixtime(d);

  /* set the file's modification time */
  SWI_OS_File_5(f,NULL,&loadaddr,NULL,NULL,&attr);

  if (uxtime2acornftime(&execaddr, &loadaddr, m_time) != 0)
      return;

  SWI_OS_File_1(f,loadaddr,execaddr,attr);
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
  unsigned int len = strlen(f);

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
  if (name[len - 1] == '.')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  if (strcmp(f, "-") == 0) {
  /* forge stat values for stdin since Amiga and RISCOS have no fstat() */
    s.st_mode = (S_IREAD|S_IWRITE|S_IFREG);
    s.st_size = -1;
    s.st_mtime = time(&s.st_mtime);
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

  return unix2dostime((time_t *) &s.st_mtime);
}

int set_extra_field(z, z_utim)
        struct zlist far *z;
        iztimes *z_utim;
{
#ifdef USE_EF_UT_TIME
  char *eb_ptr;
#endif /* USE_EF_UT_TIME */
  char *name;
  extra_block *block;

#define EB_SPARK_LEN    20
#define EB_SPARK_SIZE (EB_HEADSIZE+EB_SPARK_LEN)
#ifdef USE_EF_UT_TIME
# ifdef IZ_CHECK_TZ
#  define EB_UTTIME_SIZE (zp_tz_is_valid ? EB_HEADSIZE+EB_UT_LEN(1) : 0)
# else
#  define EB_UTTIME_SIZE (EB_HEADSIZE+EB_UT_LEN(1))
# endif
#else
#  define EB_UTTIME_SIZE 0
#endif
#define EF_SPARK_TOTALSIZE (EB_SPARK_SIZE + EB_UTTIME_SIZE)

  if ((name=(char *)malloc(strlen(z->name)+1))==NULL) {
    fprintf(stderr," set_extra_field: not enough memory for directory name\n");
    return ZE_MEM;
  }

  strcpy(name,z->name);

  if (name[strlen(name)-1]=='.') {  /* remove the last '.' in directory names */
    name[strlen(name)-1]=0;
  }

  z->extra=(char *)malloc(EF_SPARK_TOTALSIZE);
  if (z->extra==NULL) {
    fprintf(stderr," set_extra_field: not enough memory\n");
    free(name);
    return ZE_MEM;
  }
  z->cextra = z->extra;
  z->cext = z->ext = EF_SPARK_TOTALSIZE;

  block=(extra_block *)z->extra;
  block->ID=SPARKID;
  block->size=EB_SPARK_LEN;
  block->ID_2=SPARKID_2;
  block->zero=0;

  if (SWI_OS_File_5(name,NULL,&block->loadaddr,&block->execaddr,
                    NULL,&block->attr) != NULL) {
    fprintf(stderr," OS error while set_extra_field of %s\n",name);
  }

  free(name);

#ifdef USE_EF_UT_TIME
# ifdef IZ_CHECK_TZ
  if (zp_tz_is_valid) {
# endif
    eb_ptr = z->extra + EB_SPARK_SIZE;

    eb_ptr[0]  = 'U';
    eb_ptr[1]  = 'T';
    eb_ptr[2]  = EB_UT_LEN(1);          /* length of data part of e.f. */
    eb_ptr[3]  = 0;
    eb_ptr[4]  = EB_UT_FL_MTIME;
    eb_ptr[5]  = (char)(z_utim->mtime);
    eb_ptr[6]  = (char)(z_utim->mtime >> 8);
    eb_ptr[7]  = (char)(z_utim->mtime >> 16);
    eb_ptr[8]  = (char)(z_utim->mtime >> 24);
# ifdef IZ_CHECK_TZ
  }
# endif
#endif /* USE_EF_UT_TIME */

  return ZE_OK;
}

#endif /* !UTIL */


/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";

    printf(CompiledWith,
#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
#  ifdef __CC_NORCROFT
      "Norcroft ", "cc",
#  else
      "cc", "",
#  endif
#endif

      "RISC OS",

      " (Acorn Computers Ltd)",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

} /* end function version_local() */
